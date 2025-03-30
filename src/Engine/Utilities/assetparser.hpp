//
// Created by Simeon on 3/30/2025.
//

#ifndef SASSETPARSER_H
#define SASSETPARSER_H

#include <vector>
#include <mutex>
#include <chrono>
#include <cstdarg>
#include <dinput.h>
#include <string>
#include <zlib.h>
#include <sqlite3.h>
#include <fstream>
#include <filesystem>
#include <imgui.h>
#include "portable-file-dialogs.h"
#include <unordered_set>

namespace fs = std::filesystem;

struct LogEntry {
    std::string message;
    ImVec4 color;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};
#pragma pack(push, 1)
struct PackageHeader {
    char magic[4];
    uint32_t version;
    uint32_t assetCount;
};
struct AssetEntry {
    uint32_t nameLength;
    uint64_t offset;
    uint32_t compressedSize;
    uint32_t originalSize;
};
#pragma pack(pop)

namespace {
    std::vector<LogEntry> logEntries;
    const float LOG_DISPLAY_TIME = 60.0f;
    std::mutex logMutex;
    std::string currentDbPath;
    std::string currentPackagePath;
}

inline std::vector<Bytef> compressData(const std::vector<char>& input) {
    z_stream zs = {0};
    int ret = deflateInit(&zs, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        throw std::runtime_error("zlib initialization failed: " + std::to_string(ret));
    }

    try {
        zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
        zs.avail_in = static_cast<uInt>(input.size());

        uLong max_size = deflateBound(&zs, zs.avail_in);
        std::vector<Bytef> output(max_size);

        zs.next_out = output.data();
        zs.avail_out = static_cast<uInt>(output.size());

        while ((ret = deflate(&zs, Z_FINISH)) != Z_STREAM_END) {
            if (ret != Z_OK) {
                throw std::runtime_error("Compression failed: " + std::to_string(ret));
            }
        }

        output.resize(zs.total_out);
        deflateEnd(&zs);
        return output;
    } catch (...) {
        deflateEnd(&zs);
        throw;
    }
}

inline std::vector<char> decompressData(const std::vector<Bytef>& compressed, size_t originalSize) {
    z_stream zs = {0};
    int ret = inflateInit(&zs);
    if (ret != Z_OK) {
        throw std::runtime_error("zlib initialization failed: " + std::to_string(ret));
    }

    try {
        zs.next_in = const_cast<Bytef*>(compressed.data());
        zs.avail_in = static_cast<uInt>(compressed.size());

        std::vector<char> output(originalSize);
        zs.next_out = reinterpret_cast<Bytef*>(output.data());
        zs.avail_out = static_cast<uInt>(originalSize);

        // Decompress in chunks
        while ((ret = inflate(&zs, Z_NO_FLUSH)) != Z_STREAM_END) {
            if (ret != Z_OK) {
                std::string err = zs.msg ? zs.msg : "Unknown error";
                throw std::runtime_error("Decompression failed: " + err);
            }
        }

        if (zs.total_out != originalSize) {
            throw std::runtime_error("Decompressed size mismatch");
        }

        inflateEnd(&zs);
        return output;
    } catch (...) {
        inflateEnd(&zs);
        throw;
    }
}

inline void AddLog(const ImVec4& color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int size = vsnprintf(nullptr, 0, fmt, args) + 1;
    std::vector<char> buffer(size);
    vsnprintf(buffer.data(), size, fmt, args);

    va_end(args);

    std::lock_guard<std::mutex> lock(logMutex);
    logEntries.emplace_back(LogEntry{
        std::string(buffer.data()),
        color,
        std::chrono::steady_clock::now()
    });
}

struct Asset {
    int id;
    std::string name;
    std::string type;
    sqlite3_int64 offset;
    int size;
    int original_size;
};
inline void ProcessPackageFile(const fs::path& packagePath, const fs::path& outputDir) {
       AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Starting package processing: %s", packagePath.string().c_str());

    std::ifstream package(packagePath.string(), std::ios::binary | std::ios::ate);
    if (!package) {
        throw std::runtime_error("Could not open package file");
    }

    const size_t fileSize = package.tellg();
    package.seekg(0);
    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Package file size: %zu bytes", fileSize);

    try {
        PackageHeader header;
        if (!package.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            throw std::runtime_error("Failed to read package header");
        }

        AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Header read: magic=%.4s, version=%u, assets=%u",
            header.magic, header.version, header.assetCount);

        if (std::string(header.magic, 4) != "SASS") {
            char hexMagic[9];
            snprintf(hexMagic, sizeof(hexMagic), "%02X%02X%02X%02X",
                static_cast<unsigned char>(header.magic[0]),
                static_cast<unsigned char>(header.magic[1]),
                static_cast<unsigned char>(header.magic[2]),
                static_cast<unsigned char>(header.magic[3]));
            throw std::runtime_error("Invalid magic number: " + std::string(hexMagic));
        }

        if (header.assetCount == 0 || header.assetCount > 1000000) {
            throw std::runtime_error("Invalid asset count: " + std::to_string(header.assetCount));
        }

        AddLog(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Found %u assets", header.assetCount);

        for (uint32_t i = 0; i < header.assetCount; ++i) {
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Processing asset %u/%u", i + 1, header.assetCount);

            AssetEntry entry;
            if (!package.read(reinterpret_cast<char*>(&entry), sizeof(entry))) {
                throw std::runtime_error("Failed to read asset entry " + std::to_string(i));
            }

            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Asset entry: offset=%llu, compressed=%u, original=%u, name_len=%u",
                entry.offset, entry.compressedSize, entry.originalSize, entry.nameLength);

            if (entry.offset < sizeof(PackageHeader)) {
                throw std::runtime_error("Invalid offset before header end");
            }

            if (entry.nameLength == 0 || entry.nameLength > 4096) {
                throw std::runtime_error("Invalid name length: " + std::to_string(entry.nameLength));
            }

            if (entry.offset + entry.compressedSize > fileSize) {
                throw std::runtime_error("Asset data out of bounds: offset=" +
                    std::to_string(entry.offset) + " size=" + std::to_string(entry.compressedSize));
            }

            std::string assetName(entry.nameLength, '\0');
            if (!package.read(&assetName[0], entry.nameLength)) {
                throw std::runtime_error("Failed to read asset name");
            }
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Asset name: '%s'", assetName.c_str());

            const std::streampos entryEndPos = package.tellg();
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Current position: %lld", entryEndPos);

            package.seekg(entry.offset);
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Seeking to offset %llu", entry.offset);

            if (!package) {
                throw std::runtime_error("Seek failed to offset " + std::to_string(entry.offset));
            }

            std::vector<Bytef> compressedData(entry.compressedSize);
            if (!package.read(reinterpret_cast<char*>(compressedData.data()), entry.compressedSize)) {
                throw std::runtime_error("Failed to read compressed data");
            }
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Read %u compressed bytes", entry.compressedSize);
            package.seekg(entryEndPos);

            std::vector<char> finalData;
            try {
                if (entry.compressedSize != entry.originalSize) {
                    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Decompressing %u -> %u bytes",
                        entry.compressedSize, entry.originalSize);
                    finalData = decompressData(compressedData, entry.originalSize);
                } else {
                    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Using uncompressed data");
                    finalData.assign(compressedData.begin(), compressedData.end());
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Decompression failed: ") + e.what());
            }

            if (finalData.size() != entry.originalSize) {
                throw std::runtime_error("Size mismatch: expected " +
                    std::to_string(entry.originalSize) + " got " +
                    std::to_string(finalData.size()));
            }

            const fs::path outputPath = outputDir / assetName;
            AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Writing to: %s", outputPath.string().c_str());

            fs::create_directories(outputPath.parent_path());
            std::ofstream outFile(outputPath, std::ios::binary);
            if (!outFile.write(finalData.data(), finalData.size())) {
                throw std::runtime_error("Failed to write output file");
            }
            AddLog(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Successfully wrote %zu bytes", finalData.size());
        }

        AddLog(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Finished processing %u assets", header.assetCount);
    } catch (const std::exception& e) {
        const auto errorPos = package.tellg();
        AddLog(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ERROR [pos:%lld]: %s", errorPos, e.what());
        throw;
    }
}

inline bool initDB(const std::string& dbPath) {
    sqlite3* db = nullptr;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "Failed to open database: %s", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_busy_timeout(db, 3000);

    const char* sql =
        "CREATE TABLE IF NOT EXISTS assets ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT UNIQUE NOT NULL,"
        "type TEXT NOT NULL,"
        "offset INTEGER NOT NULL,"
        "size INTEGER NOT NULL,"
        "original_size INTEGER NOT NULL);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

inline std::vector<Asset> loadAssets() {
    std::vector<Asset> assets;
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_open_v2(currentDbPath.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "Failed to open database: %s", sqlite3_errmsg(db));
        return assets;
    }

    const char* sql = "SELECT id, name, type, offset, size, original_size FROM assets;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "SQL prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return assets;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        assets.push_back({
            sqlite3_column_int(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            sqlite3_column_int64(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_int(stmt, 5)
        });
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return assets;
}

inline bool extractAsset(const std::string& packagePath, const std::string& assetName,
                        const std::string& outputPath) {
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_open_v2(currentDbPath.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "Database open failed: %s", sqlite3_errmsg(db));
        return false;
    }

    const char* sql = "SELECT offset, size, original_size FROM assets WHERE name = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "SQL prepare failed: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, assetName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const auto offset = sqlite3_column_int64(stmt, 0);
        const auto compressedSize = sqlite3_column_int(stmt, 1);
        const auto originalSize = sqlite3_column_int(stmt, 2);

        std::ifstream pkg(packagePath, std::ios::binary);
        if (pkg.seekg(offset) && pkg) {
            std::vector<Bytef> compressed(compressedSize);
            if (pkg.read(reinterpret_cast<char*>(compressed.data()), compressedSize)) {
                try {
                    auto decompressed = decompressData(compressed, originalSize);
                    std::ofstream out(outputPath, std::ios::binary);
                    if (out.write(decompressed.data(), decompressed.size())) {
                        success = true;
                        AddLog(ImVec4(0,1,0,1), "Extracted %s to %s",
                              assetName.c_str(), outputPath.c_str());
                    }
                } catch (const std::exception& e) {
                    AddLog(ImVec4(1,0,0,1), "Decompression failed: %s", e.what());
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

inline bool addAsset(sqlite3* db, const std::string& assetName,
                    const std::string& filePath, std::ofstream& package) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        AddLog(ImVec4(1,0,0,1), "Failed to open %s", filePath.c_str());
        return false;
    }

    const auto originalSize = static_cast<size_t>(file.tellg());
    file.seekg(0);
    std::vector<char> input(originalSize);
    if (!file.read(input.data(), originalSize)) {
        AddLog(ImVec4(1,0,0,1), "Failed to read %s", filePath.c_str());
        return false;
    }

    std::vector<Bytef> compressed;
    try {
        compressed = compressData(input);
    } catch (const std::exception& e) {
        AddLog(ImVec4(1,0,0,1), "Compression failed: %s", e.what());
        return false;
    }

    const auto offset = package.tellp();
    if (!package.write(reinterpret_cast<const char*>(compressed.data()), compressed.size())) {
        AddLog(ImVec4(1,0,0,1), "Package write failed at offset %lld", offset);
        return false;
    }

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO assets (name, type, offset, size, original_size) "
                     "VALUES (?1, 'auto', ?2, ?3, ?4);";

    sqlite3_stmt* checkStmt;
    const char* checkSql = "SELECT 1 FROM assets WHERE name = ?1;";
    if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "DB check failed: %s", sqlite3_errmsg(db));
        return false;
    }
    sqlite3_bind_text(checkStmt, 1, assetName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        AddLog(ImVec4(1,0,0,1), "Asset '%s' already exists!", assetName.c_str());
        sqlite3_finalize(checkStmt);
        return false;
    }
    sqlite3_finalize(checkStmt);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        AddLog(ImVec4(1,0,0,1), "SQL prepare failed: %s", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, assetName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, offset);
    sqlite3_bind_int(stmt, 3, static_cast<int>(compressed.size()));
    sqlite3_bind_int(stmt, 4, static_cast<int>(originalSize));

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        AddLog(ImVec4(1,0,0,1), "Insert failed: %s", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    AddLog(ImVec4(0,1,0,1), "Added %s (%zu => %zu bytes)",
          assetName.c_str(), originalSize, compressed.size());

    AssetEntry entry;
    entry.nameLength = static_cast<uint32_t>(assetName.size());
    entry.offset = offset;
    entry.compressedSize = static_cast<uint32_t>(compressed.size());
    entry.originalSize = static_cast<uint32_t>(originalSize);

    package.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
    package.write(assetName.data(), assetName.size());

    return true;
}

inline void ShowAssetManagerUI(bool* p_open) {
    if (ImGui::Begin("Asset Manager", p_open)) {

        static std::vector<Asset> assets;
        static std::vector<std::pair<std::string, std::string>> assetsToAdd;

        static char packageName[256] = "game";
        static char outputDir[256] = "output/";
        static bool useCustomDir = false;
        static char newAssetPrefix[128] = "";
        static char newAssetPath[256] = "";

        ImGui::Text("Package Configuration");
        ImGui::InputText("Package Name", packageName, IM_ARRAYSIZE(packageName));
        ImGui::Checkbox("Custom Output Directory", &useCustomDir);
        if (useCustomDir) {
            ImGui::InputText("Output Path", outputDir, IM_ARRAYSIZE(outputDir));
        }

        if (ImGui::Button("Initialize Package")) {
            const fs::path dirPath = useCustomDir ? fs::path(outputDir) : fs::path();
            if (!dirPath.empty() && !fs::create_directories(dirPath)) {
                AddLog(ImVec4(1,0,0,1), "Failed to create directory: %s", dirPath.string().c_str());
            } else {
                currentDbPath = (dirPath / (std::string(packageName) + ".db")).string();
                currentPackagePath = (dirPath / (std::string(packageName) + ".sassets")).string();

                if (initDB(currentDbPath)) {
                    assets = loadAssets();
                    AddLog(ImVec4(0,1,0,1), "Created new package: %s", currentPackagePath.c_str());
                }
            }
        }

        ImGui::Separator();

        if (assets.empty()) {
            ImGui::TextColored(ImVec4(1,0.5,0,1), "No assets loaded. Initialize a package and add assets first.");
        }
        else {
            if (ImGui::BeginTable("Assets", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100);
                ImGui::TableHeadersRow();

                for (const auto& asset : assets) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", asset.id);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", asset.name.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", asset.type.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.2f KB", asset.size / 1024.0f);
                    ImGui::TableSetColumnIndex(4);

                    ImGui::PushID(asset.id);
                    if (ImGui::Button("Extract")) {
                        const auto outPath = fs::path("extracted") / asset.name;
                        if (!fs::exists(outPath.parent_path())) {
                            fs::create_directories(outPath.parent_path());
                        }
                        if (extractAsset(currentPackagePath, asset.name, outPath.string())) {
                            AddLog(ImVec4(0,1,0,1), "Extracted: %s", asset.name.c_str());
                        } else {
                            AddLog(ImVec4(1,0,0,1), "Failed to extract: %s", asset.name.c_str());
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }

        ImGui::Separator();

        ImGui::Text("Add Assets");
        ImGui::InputText("Asset Prefix", newAssetPrefix, IM_ARRAYSIZE(newAssetPrefix));
        ImGui::InputText("File/Directory", newAssetPath, IM_ARRAYSIZE(newAssetPath));

        if (ImGui::Button("Add Assets")) {
            try {

                std::unordered_set<std::string> allNames;

                sqlite3* tempDb;
                if (sqlite3_open(currentDbPath.c_str(), &tempDb) == SQLITE_OK) {
                    sqlite3_stmt* stmt;
                    sqlite3_prepare_v2(tempDb, "SELECT name FROM assets;", -1, &stmt, nullptr);
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        allNames.insert(
                            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
                        );
                    }
                    sqlite3_finalize(stmt);
                    sqlite3_close(tempDb);
                }

                for (const auto& [name, path] : assetsToAdd) {
                    allNames.insert(name);
                }

                const fs::path sourcePath(newAssetPath);
                if (!fs::exists(sourcePath)) {
                    AddLog(ImVec4(1,0,0,1), "Path not found: %s", newAssetPath);
                } else if (fs::is_directory(sourcePath)) {
                    size_t count = 0;
                    for (const auto& entry : fs::recursive_directory_iterator(sourcePath)) {
                        if (entry.is_regular_file()) {
                            const auto relPath = fs::relative(entry.path(), sourcePath).generic_string();
                            const auto assetName = std::string(newAssetPrefix) + "/" + relPath;
                            assetsToAdd.emplace_back(assetName, entry.path().string());
                            count++;
                        }
                    }
                    AddLog(ImVec4(0,1,0,1), "Added %zu files from directory", count);
                } else {
                    const std::string assetName = newAssetPrefix[0] ?
                        std::string(newAssetPrefix) : sourcePath.filename().string();
                    assetsToAdd.emplace_back(assetName, sourcePath.string());
                    AddLog(ImVec4(0,1,0,1), "Added single file: %s", assetName.c_str());
                }
            } catch (const fs::filesystem_error& e) {
                AddLog(ImVec4(1,0,0,1), "Filesystem error: %s", e.what());
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Build Package")) {
            if (assetsToAdd.empty()) {
                AddLog(ImVec4(1,1,0,1), "No assets to add!");
            } else if (currentPackagePath.empty()) {
                AddLog(ImVec4(1,0,0,1), "Initialize package first!");
            } else {
                sqlite3* db = nullptr;
                std::string tmpPackage = currentPackagePath + ".tmp";
                bool success = true;

                try {
                    std::unordered_set<std::string> existingNames;
                    sqlite3_stmt* stmt;
                    sqlite3_prepare_v2(db, "SELECT name FROM assets;", -1, &stmt, nullptr);
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        existingNames.insert(
                            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
                        );
                    }
                    sqlite3_finalize(stmt);

                    std::vector<std::pair<std::string, std::string>> filteredAssets;
                    std::unordered_set<std::string> newNames;

                    for (const auto& [name, path] : assetsToAdd) {
                        if (existingNames.count(name) || newNames.count(name)) {
                            AddLog(ImVec4(1,0.5,0,1), "Skipping duplicate: %s", name.c_str());
                        } else {
                            filteredAssets.push_back({name, path});
                            newNames.insert(name);
                        }
                    }

                    assetsToAdd.swap(filteredAssets);

                    if (assetsToAdd.empty()) {
                        throw std::runtime_error("No non-duplicate assets to add");
                    }
                    if (sqlite3_open(currentDbPath.c_str(), &db) != SQLITE_OK) {
                        throw std::runtime_error(sqlite3_errmsg(db));
                    }

                    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK) {
                        throw std::runtime_error(sqlite3_errmsg(db));
                    }

                    {
                        std::ofstream package(tmpPackage, std::ios::binary | std::ios::trunc);
                        if (!package) {
                            throw std::runtime_error("Failed to create temporary package file");
                        }

                        size_t totalEntriesSize = 0;
                        for (const auto& [name, path] : assetsToAdd) {
                            totalEntriesSize += sizeof(AssetEntry) + name.size();
                        }

                        uint64_t current_offset = sizeof(PackageHeader) + totalEntriesSize;

                        std::vector<AssetEntry> entries;
                        std::vector<std::string> names;
                        std::vector<std::vector<Bytef>> compressedDataList;

                        for (const auto& [name, path] : assetsToAdd) {
                            std::ifstream file(path, std::ios::binary | std::ios::ate);
                            size_t originalSize = file.tellg();
                            file.seekg(0);
                            std::vector<char> input(originalSize);
                            file.read(input.data(), originalSize);

                            auto compressed = compressData(input);
                            compressedDataList.push_back(compressed);

                            AssetEntry entry;
                            entry.nameLength = static_cast<uint32_t>(name.size());
                            entry.offset = current_offset;
                            entry.compressedSize = static_cast<uint32_t>(compressed.size());
                            entry.originalSize = static_cast<uint32_t>(originalSize);

                            entries.push_back(entry);
                            names.push_back(name);

                            current_offset += compressed.size();
                        }

                        PackageHeader header;
                        memcpy(header.magic, "SASS", 4);
                        header.version = 1;
                        header.assetCount = static_cast<uint32_t>(assetsToAdd.size());
                        package.write(reinterpret_cast<const char*>(&header), sizeof(header));

                        for (size_t i = 0; i < entries.size(); ++i) {
                            package.write(reinterpret_cast<const char*>(&entries[i]), sizeof(AssetEntry));
                            package.write(names[i].data(), names[i].size());
                        }

                        for (const auto& compressed : compressedDataList) {
                            package.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
                        }
                    }

                    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
                        throw std::runtime_error(sqlite3_errmsg(db));
                    }

                    if (fs::exists(currentPackagePath)) {
                        if (!fs::remove(currentPackagePath)) {
                            throw std::runtime_error("Could not remove old package file");
                        }
                    }

#ifdef _WIN32
                    if (!MoveFileExA(tmpPackage.c_str(), currentPackagePath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
                        throw std::runtime_error("Failed to replace package file");
                    }
#else
                    fs::rename(tmpPackage, currentPackagePath);
#endif

                    size_t added_count = assetsToAdd.size();
                    assetsToAdd.clear();
                    AddLog(ImVec4(0,1,0,1), "Built package with %zu assets", added_count);
                }
                catch (const std::exception& e) {
                    success = false;
                    if (db) sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

                    if (fs::exists(tmpPackage)) {
                        fs::remove(tmpPackage);
                    }

                    AddLog(ImVec4(1,0,0,1), "Build failed: %s", e.what());
                }

                if (db) {
                    sqlite3_close(db);
                }
            }
        }

        ImGui::Separator();
        ImGui::Text("Logs");
        ImGui::BeginChild("Logs", ImVec2(0, 150), true);
        {
            std::lock_guard<std::mutex> lock(logMutex);
            const auto now = std::chrono::steady_clock::now();

            for (auto it = logEntries.begin(); it != logEntries.end();) {
                const float age = std::chrono::duration<float>(now - it->timestamp).count();

                if (age > LOG_DISPLAY_TIME) {
                    it = logEntries.erase(it);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, it->color);
                    ImGui::TextWrapped("[%.1fs] %s", LOG_DISPLAY_TIME - age, it->message.c_str());
                    ImGui::PopStyleColor();
                    ++it;
                }
            }
        }
        ImGui::EndChild();

        if (ImGui::Button("Decompress .sassets")) {
            try {
                AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Opening file dialog...");
                auto dialog = pfd::open_file(
                    "Select Asset Package",
                    "",
                    {"Asset Packages (.sassets)", "*.sassets"},
                    pfd::opt::none
                );


                if (!dialog.result().empty()) {
                    const fs::path packagePath = dialog.result()[0];
                    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Selected file: %s", packagePath.string().c_str());

                    if (!fs::exists(packagePath)) {
                        throw std::runtime_error("File does not exist: " + packagePath.string());
                    }

                    fs::path outputDir = packagePath.parent_path() /
                                       (packagePath.stem().string() + "_decompressed");
                    int counter = 1;
                    while (fs::exists(outputDir)) {
                        outputDir = packagePath.parent_path() /
                                  (packagePath.stem().string() + "_decompressed_" + std::to_string(counter++));
                    }

                    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Creating output directory: %s", outputDir.string().c_str());
                    fs::create_directories(outputDir);

                    if (!fs::exists(outputDir)) {
                        throw std::runtime_error("Failed to create output directory");
                    }

                    AddLog(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Starting decompression...");
                    ProcessPackageFile(packagePath, outputDir);
                    AddLog(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Decompression completed successfully!");

#ifdef _WIN32
                    ShellExecuteA(nullptr, "open", outputDir.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
#endif
                }
                else {
                    AddLog(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "File selection canceled");
                }
            } catch (const std::exception& e) {
                AddLog(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ERROR: %s", e.what());
            }
        }
    }

    ImGui::End();
}

#endif