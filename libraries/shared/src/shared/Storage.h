//
//  Created by Bradley Austin Davis on 2016/02/17
//  Copyright 2013-2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once
#ifndef hifi_Storage_h
#define hifi_Storage_h

#include <stdint.h>
#include <vector>
#include <memory>
#include <QFile>
#include <QString>

namespace storage {
    class Storage;
    using StoragePointer = std::shared_ptr<const Storage>;

    class Storage : public std::enable_shared_from_this<Storage> {
    public:
        virtual ~Storage() {}
        virtual const uint8_t* data() const = 0;
        virtual size_t size() const = 0;

        StoragePointer createView(size_t size = 0, size_t offset = 0) const;
        StoragePointer toFileStorage(const QString& filename) const;
        StoragePointer toMemoryStorage() const;

        // Aliases to prevent having to re-write a ton of code
        inline size_t getSize() const { return size(); }
        inline const uint8_t* readData() const { return data(); }
    };

    class MemoryStorage : public Storage {
    public:
        MemoryStorage(size_t size, const uint8_t* data = nullptr);
        const uint8_t* data() const override { return _data.data(); }
        uint8_t* data() { return _data.data(); }
        size_t size() const override { return _data.size(); }
    private:
        std::vector<uint8_t> _data;
    };

    class FileStorage : public Storage {
    public:
        static StoragePointer create(const QString& filename, size_t size, const uint8_t* data);
        FileStorage(const QString& filename);
        ~FileStorage();
        // Prevent copying
        FileStorage(const FileStorage& other) = delete;
        FileStorage& operator=(const FileStorage& other) = delete;

        const uint8_t* data() const override { return _mapped; }
        size_t size() const override { return _file.size(); }
    private:
        QFile _file;
        uint8_t* _mapped { nullptr };
    };

    class ViewStorage : public Storage {
    public:
        ViewStorage(const storage::StoragePointer& owner, size_t size, const uint8_t* data);
        const uint8_t* data() const override { return _data; }
        size_t size() const override { return _size; }
    private:
        const storage::StoragePointer _owner;
        const size_t _size;
        const uint8_t* _data;
    };

}

#endif // hifi_Storage_h