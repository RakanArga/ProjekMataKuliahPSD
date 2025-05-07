#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;
using namespace std::chrono;

// Struktur Data
struct Tugas {
    string namaTugas;
    string namaMatkul;
    string deadline; // Format "YYYY-MM-DD"
    bool status;
    string detailTugas;
    string kategori;
    time_point<system_clock> waktuMulai;
    Tugas* next = nullptr;
};

struct Stack {
    Tugas* snapshot;
    Stack* top;
};

// Global Variabel
Tugas* depan = nullptr;
Stack* undoTop = nullptr;
Stack* redoTop = nullptr;
const int MAX_CATEGORIES = 10;
string categories[MAX_CATEGORIES] = {"perkuliahan", "projek", "praktek"};
int categoryCount = 3;

// Fungsi Tambahan
time_point<system_clock> parsingDeadline(string& deadline) {
    struct tm Time = {};
    stringstream parse(deadline);
    parse >> get_time(&Time, "%Y-%m-%d");
    if (parse.fail()) {
        throw runtime_error("Failed to parse...");
    }
    time_t parse2 = mktime(&Time);
    return system_clock::from_time_t(parse2);
}

bool displayTimeRemaining(Tugas& task) {
    auto deadlineTime = parsingDeadline(task.deadline);
    auto now = system_clock::now();
    if (now < deadlineTime) {
        auto remaining = deadlineTime - now;
        auto jam = duration_cast<hours>(remaining).count();
        auto hari = jam / 24;
        jam %= 24;
        cout << "Sisa Waktu Buat Tugas " << task.namaTugas
             << " - Sisa: " << hari << " hari dan " << jam << " jam.\n";
        return true;
    } else {
        cout << "❌ Deadline tugas \"" << task.namaTugas << "\" sudah lewat!\n";
        return false;
    }
}

// Fungsi Stack
Tugas* copyList(Tugas* task) {
    if (task == nullptr) {
        return nullptr;
    }
    Tugas* pbaru = new Tugas;
    pbaru->namaTugas = task->namaTugas;
    pbaru->namaMatkul = task->namaMatkul;
    pbaru->deadline = task->deadline;
    pbaru->status = task->status;
    pbaru->waktuMulai = task->waktuMulai;
    pbaru->next = nullptr;

    Tugas* helper = task->next;
    Tugas* currentCopy = pbaru;
    while (helper != nullptr) {
        Tugas* baru = new Tugas;
        baru->namaTugas = helper->namaTugas;
        baru->namaMatkul = helper->namaMatkul;
        baru->deadline = helper->deadline;
        baru->status = helper->status;
        baru->waktuMulai = helper->waktuMulai;
        baru->next = nullptr;
        currentCopy->next = baru;
        currentCopy = baru;
        helper = helper->next;
    }
    return pbaru;
}

void push(Stack*& top, Tugas* snapshot) {
    Stack* stackBaru = new Stack;
    stackBaru->snapshot = snapshot;
    stackBaru->top = top;
    top = stackBaru;
}

Tugas* pop(Stack*& top) {
    if (top == nullptr) {
        cout << "Stack Nya Kosong Bro" << endl;
        return nullptr;
    }
    Stack* temp = top;
    Tugas* snapshot = temp->snapshot;
    top = top->top;
    delete temp;
    return snapshot;
}

void undo(Stack*& undoStack, Stack*& redoStack, Tugas*& snapshot) {
    if (undoStack == nullptr) {
        cout << "Hmm.. Kosong" << endl;
        return;
    }
    push(redoStack, copyList(snapshot));
    Tugas* statesebelum = pop(undoStack);
    depan = statesebelum;
    cout << "Berhasil Undo..\n";
}

void redo(Stack*& undoStack, Stack*& redoStack, Tugas*& snapshot) {
    if (redoStack == nullptr) {
        cout << "Hmm.. Kosong" << endl;
        return;
    }
    push(undoStack, copyList(snapshot));
    Tugas* stateberikut = pop(redoStack);
    depan = stateberikut;
    cout << "Berhasil Redo..\n";
}

void clearStack(Stack*& top) {
    while (top != nullptr) {
        Tugas* t = top->snapshot;
        while (t != nullptr) {
            Tugas* tmp = t;
            t = t->next;
            delete tmp;
        }
        Stack* tmpStack = top;
        top = top->top;
        delete tmpStack;
    }
}

// Fungsi Tugas
void tambahTugas(string namaTugas, string namaMatkul, string deadline, bool status) {
    push(undoTop, copyList(depan));
    Tugas* tugasBaru = new Tugas;
    tugasBaru->namaTugas = namaTugas;
    tugasBaru->namaMatkul = namaMatkul;
    tugasBaru->deadline = deadline;
    tugasBaru->status = status;
    tugasBaru->waktuMulai = system_clock::now();
    if (depan == NULL) {
        depan = tugasBaru;
    } else {
        Tugas* helper = depan;
        while (helper->next != NULL) {
            helper = helper->next;
        }
        helper->next = tugasBaru;
    }
}

void hapusTugas(string namaTugas) {
    push(undoTop, copyList(depan));
    if (depan == NULL) {
        cout << "Gaada Tugas Disini\n";
        return;
    }
    if (depan->namaTugas == namaTugas) {
        Tugas* temp = depan;
        depan = depan->next;
        delete temp;
        cout << "Tugas Dihapus..\n";
        return;
    }
    Tugas* helper = depan;
    while (helper->next != nullptr && helper->next->namaTugas != namaTugas) {
        helper = helper->next;
    }
    if (helper->next != NULL) {
        Tugas* temp = helper->next;
        helper->next = helper->next->next;
        delete temp;
        cout << "Tugas: " << namaTugas << " Udah Kehapus\n";
    } else {
        cout << "Tugas itu gaada bro\n";
    }
}

void displayTugas() {
    if (depan == NULL) {
        cout << "Tidak Ada Tugas.\n";
        return;
    }
    Tugas* helper = depan;
    cout << "Daftar Semua Tugas:\n";
    while (helper != nullptr) {
        cout << "----------------------------\n";
        cout << "Tugas       : " << helper->namaTugas << endl;
        cout << "Mata Kuliah : " << helper->namaMatkul << endl;
        cout << "Deadline    : " << helper->deadline << endl;
        cout << "Status      : " << (helper->status ? "selesai" : "belum selesai") << endl;
        displayTimeRemaining(*helper);
        helper = helper->next;
    }
}

// Main Program
int main() {
    int pilihan;
    string namaTugas, namaMatkul, deadline;
    bool status;
    cout << "Selamat Datang Di SantaiTapiJalan\n";
    cout << "Mau Numpuk Tugas Lagi Yeee?\n";
    do {
        cout << "\nMenu:\n";
        cout << "1. Tambah Tugas\n";
        cout << "2. Hapus Tugas\n";
        cout << "3. Tampilkan Semua Tugas\n";
        cout << "4. Undo\n";
        cout << "5. Redo\n";
        cout << "6. Keluar\n";
        cout << "Pilih opsi (1-6): ";
        cin >> pilihan;

        switch (pilihan) {
        case 1:
            cout << "Masukkan Nama Tugas: ";
            cin.ignore();
            getline(cin, namaTugas);
            cout << "Masukkan Nama Mata Kuliah: ";
            getline(cin, namaMatkul);
            cout << "Masukkan Deadline (YYYY-MM-DD): ";
            getline(cin, deadline);
            cout << "Apakah tugas sudah selesai? (1 untuk ya, 0 untuk tidak): ";
            cin >> status;
            tambahTugas(namaTugas, namaMatkul, deadline, status);
            break;

        case 2:
            cout << "Masukkan Nama Tugas yang ingin dihapus: ";
            cin.ignore();
            getline(cin, namaTugas);
            hapusTugas(namaTugas);
            break;

        case 3:
            displayTugas();
            break;

        case 4:
            undo(undoTop, redoTop, depan);
            break;

        case 5:
            redo(undoTop, redoTop, depan);
            break;

        case 6:
            cout << "Keluar dari aplikasi.\n";
            clearStack(undoTop);
            clearStack(redoTop);
            break;

        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            break;
        }
    } while (pilihan != 6);

    return 0;
}