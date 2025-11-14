#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

const int DISK_SIZE = 4096;

struct FileEntry {
    std::string name;
    int start;    // индекс в "диске"
    int size;     // выделенный размер в байтах
    bool used;    // существует ли файл
};

std::vector<char> disk(DISK_SIZE, 0);
std::vector<FileEntry> directory;

// поиск файла по имени
int findFile(const std::string &name) {
    for (size_t i = 0; i < directory.size(); ++i) {
        if (directory[i].used && directory[i].name == name)
            return (int)i;
    }
    return -1;
}

// проверить, свободен ли диапазон [start, start+size)
bool isFreeRange(int start, int size) {
    int end = start + size;
    if (end > DISK_SIZE) return false;
    for (const auto &f : directory) {
        if (!f.used) continue;
        int fStart = f.start;
        int fEnd = f.start + f.size;
        // пересечение отрезков
        if (!(end <= fStart || start >= fEnd)) {
            return false;
        }
    }
    return true;
}

// найти свободный непрерывный участок заданного размера
int findFreeBlock(int size) {
    for (int start = 0; start + size <= DISK_SIZE; ++start) {
        if (isFreeRange(start, size))
            return start;
    }
    return -1;
}

void cmd_create(const std::string &name, int size) {
    if (findFile(name) != -1) {
        std::cout << "file with this name already exist\n";
        return;
    }
    int start = findFreeBlock(size);
    if (start == -1) {
        std::cout << "No space on disk for file\n";
        return;
    }
    FileEntry f{ name, start, size, true };
    directory.push_back(f);
    // обнулим содержимое
    std::fill(disk.begin() + start, disk.begin() + start + size, 0);
    std::cout << "File was created: " << name << " (start=" << start << ", size=" << size << ")\n";
}

void cmd_delete(const std::string &name) {
    int idx = findFile(name);
    if (idx == -1) {
        std::cout << "File was not found\n";
        return;
    }
    directory[idx].used = false;
    std::cout << "File was deleted: " << name << "\n";
}

void cmd_write(const std::string &name, const std::string &data) {
    int idx = findFile(name);
    if (idx == -1) {
        std::cout << "File was not found\n";
        return;
    }
    FileEntry &f = directory[idx];
    int len = (int)std::min((int)data.size(), f.size);
    // записываем в начало файла
    for (int i = 0; i < len; ++i) {
        disk[f.start + i] = data[i];
    }
    // остальное обнулим
    for (int i = len; i < f.size; ++i) {
        disk[f.start + i] = 0;
    }
    std::cout << "Recorded " << len << " bytes into file " << name << "\n";
}

void cmd_read(const std::string &name) {
    int idx = findFile(name);
    if (idx == -1) {
        std::cout << "File was not found\n";
        return;
    }
    FileEntry &f = directory[idx];
    std::string content;
    content.reserve(f.size);
    for (int i = 0; i < f.size; ++i) {
        char c = disk[f.start + i];
        if (c == 0) break; // считаем, что дальше "пусто"
        content.push_back(c);
    }
    std::cout << "File contents " << name << ":\n";
    std::cout << content << "\n";
}

void cmd_copy(const std::string &src, const std::string &dst) {
    int sidx = findFile(src);
    if (sidx == -1) {
        std::cout << "Source file was not found\n";
        return;
    }
    if (findFile(dst) != -1) {
        std::cout << "Destination file already exist\n";
        return;
    }
    FileEntry &sf = directory[sidx];

    int start = findFreeBlock(sf.size);
    if (start == -1) {
        std::cout << "No space for coppying\n";
        return;
    }

    FileEntry df{ dst, start, sf.size, true };
    directory.push_back(df);

    // копируем байты
    for (int i = 0; i < sf.size; ++i) {
        disk[start + i] = disk[sf.start + i];
    }
    std::cout << "File" << src << " was coppied to " << dst << "\n";
}

void cmd_move(const std::string &oldName, const std::string &newName) {
    int idx = findFile(oldName);
    if (idx == -1) {
        std::cout << "File was not found\n";
        return;
    }
    if (findFile(newName) != -1) {
        std::cout << "File with this name " << newName << " already exist\n";
        return;
    }
    directory[idx].name = newName;
    std::cout << "File name was changed: " << oldName << " -> " << newName << "\n";
}

void cmd_dump() {
    std::cout << "Dump of file system\n";
    std::cout << "Size of the disk: " << DISK_SIZE << " byte\n";
    std::cout << "Files:\n";
    for (const auto &f : directory) {
        if (!f.used) continue;
        std::cout << "- " << f.name
                  << " [start=" << f.start
                  << ", size=" << f.size << "]\n";
        std::cout << "  Content: \"";
        for (int i = 0; i < f.size; ++i) {
            char c = disk[f.start + i];
            if (c == 0) break;
            if (c == '\n') std::cout << "\\n";
            else std::cout << c;
        }
        std::cout << "\"\n";
    }
    std::cout << "===== End of the dump =====\n";
}

void print_help() {
    std::cout << "Commands:\n";
    std::cout << "  create <name> <size>\n";
    std::cout << "  delete <name>\n";
    std::cout << "  write <name> <text>\n";
    std::cout << "  read <name>\n";
    std::cout << "  copy <src> <dst>\n";
    std::cout << "  move <old> <new>\n";
    std::cout << "  dump\n";
    std::cout << "  help\n";
    std::cout << "  exit\n";
}

int main() {
    std::string line;
    std::cout << "Model of file system activated!\n";
    print_help();
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "create") {
            std::string name;
            int size;
            if (!(iss >> name >> size)) {
                std::cout << "Using: create <name> <size>\n";
            } else {
                cmd_create(name, size);
            }
        } else if (cmd == "delete") {
            std::string name;
            if (!(iss >> name)) {
                std::cout << "Using: delete <name>\n";
            } else {
                cmd_delete(name);
            }
        } else if (cmd == "write") {
            std::string name;
            if (!(iss >> name)) {
                std::cout << "Using: write <name> <text>\n";
            } else {
                std::string data;
                std::getline(iss, data); // остаток строки как текст
                if (!data.empty() && data[0] == ' ')
                    data.erase(0, 1);
                cmd_write(name, data);
            }
        } else if (cmd == "read") {
            std::string name;
            if (!(iss >> name)) {
                std::cout << "Using: read <name>\n";
            } else {
                cmd_read(name);
            }
        } else if (cmd == "copy") {
            std::string s, d;
            if (!(iss >> s >> d)) {
                std::cout << "Using: copy <src> <dst>\n";
            } else {
                cmd_copy(s, d);
            }
        } else if (cmd == "move") {
            std::string o, n;
            if (!(iss >> o >> n)) {
                std::cout << "Using: move <old> <new>\n";
            } else {
                cmd_move(o, n);
            }
        } else if (cmd == "dump") {
            cmd_dump();
        } else if (cmd == "help") {
            print_help();
        } else if (cmd == "exit") {
            break;
        } else {
            std::cout << "Unknown command. Use help to see all the commands.\n";
        }
    }
    return 0;
}
