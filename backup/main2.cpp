#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// LIST LIST ERROR YANG HARUS DI FIX
// TODO:
// 1. Nambahin 1 Struktur data lagi
// 2. Bikin error handler buat tanggal
// 3. Satuin file queue, detail tugas, array kategori

// struct buat data data
struct Tugas
{
    string namaTugas;
    string namaMatkul;
    string deadline; // Format "YYYY-MM-DD"
    bool status;
    string detailTugas;
    string kategori;
    time_point<system_clock> waktuMulai;
    Tugas *next = nullptr;
};

struct stack
{
    Tugas *snapshot;
    stack *top;
};

// Global Variabel
Tugas *depan = nullptr;
stack *undoTop = nullptr;
stack *redoTop = nullptr;
const int MAX_CATEGORIES = 10;
string categories[MAX_CATEGORIES] = {"perkuliahan", "projek", "praktek"};
int categoryCount = 3;

time_point<system_clock> parsingDeadline(string &deadline);
bool displayTimeRemaining(Tugas &task);

// Array Buat Kategori
void tambahKategori(string newCategory)
{
    if (categoryCount < MAX_CATEGORIES)
    {
        // Cek apakah kategori sudah ada
        for (int i = 0; i < categoryCount; i++)
        {
            if (categories[i] == newCategory)
            {
                cout << "Kategori '" << newCategory << "' sudah ada.\n";
                return;
            }
        }
        categories[categoryCount] = newCategory;
        categoryCount++;
        cout << "Kategori '" << newCategory << "' berhasil ditambahkan.\n";
    }
    else
    {
        cout << "Jumlah kategori sudah mencapai batas maksimum.\n";
    }
}
// Fungsi untuk menampilkan semua kategori
void displayKategori()
{
    cout << "Daftar Kategori:\n";
    for (int i = 0; i < categoryCount; i++)
    {
        cout << i + 1 << ". " << categories[i] << endl;
    }
}

// Fungsi untuk memilih kategori dari array
string pilihKategori()
{
    int pilihan;
    displayKategori();
    cout << categoryCount + 1 << ". Buat kategori baru\n";
    cout << "Pilih kategori (1-" << categoryCount + 1 << "): ";
    cin >> pilihan;

    if (pilihan > 0 && pilihan <= categoryCount)
    {
        return categories[pilihan - 1];
    }
    else if (pilihan == categoryCount + 1)
    {
        string newCategory;
        cout << "Masukkan kategori baru: ";
        cin.ignore();
        getline(cin, newCategory);
        tambahKategori(newCategory);
        return newCategory;
    }
    else
    {
        cout << "Pilihan tidak valid. Menggunakan kategori 'Lainnya'.\n";
        return "Lainnya";
    }
}
// Menambahkan Detail Tugas
// Fungsi untuk menampilkan detail tugas berdasarkan nama tugas
void displayDetailTugas(string namaTugasCari)
{
    bool ditemukan = false;
    Tugas *helper = depan;

    while (helper != NULL)
    {
        if (helper->namaTugas == namaTugasCari)
        {
            cout << "----------------------------\n";
            cout << "Detail Tugas: " << helper->namaTugas << endl;
            cout << "----------------------------\n";
            cout << "Nama Tugas   : " << helper->namaTugas << endl;
            cout << "Mata Kuliah  : " << helper->namaMatkul << endl;
            cout << "Deadline     : " << helper->deadline << endl;
            cout << "Status       : " << (helper->status ? "selesai" : "belum selesai") << endl;
            cout << "Detail Tugas : " << helper->detailTugas << endl;
            displayTimeRemaining(*helper);
            ditemukan = true;
            break;
        }
        helper = helper->next;
    }
    if (!ditemukan)
    {
        cout << "Tugas dengan nama \"" << namaTugasCari << "\" tidak ditemukan.\n";
    }
}

// Fungsi untuk menambahkan detail ke tugas yang sudah ada
void tambahDetailTugas(string namaTugasCari, string detailBaru)
{
    bool ditemukan = false;
    Tugas *helper = depan;

    while (helper != NULL)
    {
        if (helper->namaTugas == namaTugasCari)
        {
            helper->detailTugas = detailBaru;
            cout << "----------------------------\n";
            cout << "Detail tugas untuk \"" << namaTugasCari << "\" berhasil ditambahkan.\n";
            cout << "Tugas       : " << helper->namaTugas << endl;
            cout << "Mata Kuliah : " << helper->namaMatkul << endl;
            cout << "Deadline    : " << helper->deadline << endl;
            cout << "Status      : " << (helper->status ? "selesai" : "belum selesai") << endl;
            cout << "Detail Tugas: " << helper->detailTugas << endl;
            displayTimeRemaining(*helper); // Tambahkan tampilan waktu tersisa
            ditemukan = true;
            break;
        }
        helper = helper->next;
    }
    if (!ditemukan)
    {
        cout << "Tugas dengan nama \"" << namaTugasCari << "\" tidak ditemukan.\n";
    }
}
// Stack
Tugas *copyList(Tugas *task)
{
    if (task == nullptr)
    {
        return nullptr;
    }
    // Ini tuh kopas kepala dari linked list satu ke linked list baru
    Tugas *pbaru = new Tugas;
    pbaru->namaTugas = task->namaTugas;
    pbaru->namaMatkul = task->namaMatkul;
    pbaru->deadline = task->deadline;
    pbaru->status = task->status;
    pbaru->waktuMulai = task->waktuMulai;
    pbaru->next = nullptr;
    /*
    node buat traversal
    helper buat traverse ke linked list lama
    currentcopy traverse yang baru
    */
    Tugas *helper = task->next;
    Tugas *currentCopy = pbaru;
    while (helper != nullptr)
    {
        // bakal terus copy sampe null pointer.
        Tugas *baru = new Tugas;
        baru->namaTugas = helper->namaTugas;
        baru->namaMatkul = helper->namaMatkul;
        baru->deadline = helper->deadline;
        baru->status = helper->status;
        baru->waktuMulai = helper->waktuMulai;
        baru->next = nullptr;
        // Menyambungkan lalu current copy traverse ke node berikutnya
        // helper kemudian lanjut buat copas lagi
        currentCopy->next = baru;
        currentCopy = baru;
        helper = helper->next;
    }
    return pbaru;
}

void push(stack *&top, Tugas *snapshot)
{
    stack *stackBaru = new stack;
    stackBaru->snapshot = snapshot;
    stackBaru->top = top;
    top = stackBaru;
}

Tugas *pop(stack *&top)
{
    if (top == nullptr)
    {
        cout << "Stack Nya Kosong Bro" << endl;
        return nullptr;
    }
    stack *temp = top;
    Tugas *snapshot = temp->snapshot;
    top = top->top;
    delete (temp);
    return snapshot;
}

void undo(stack *&undoStack, stack *&redoStack, Tugas *&snapshot)
{
    if (undoStack == nullptr)
    {
        cout << "Hmm.. Kosong" << endl;
        return;
    }

    push(redoStack, copyList(snapshot));
    Tugas *statesebelum = pop(undoStack);
    depan = statesebelum;
    cout << "Berhasil Undo..\n";
}

void redo(stack *&undoStack, stack *&redoStack, Tugas *&snapshot)
{
    if (redoStack == nullptr)
    {
        cout << "Hmm.. Kosong" << endl;
        return;
    }

    push(undoStack, copyList(snapshot));

    Tugas *stateberikut = pop(redoStack);
    depan = stateberikut;
    cout << "Berhasil Redos..\n";
}
void clearStack(stack *&top)
{
    while (top != nullptr)
    {
        Tugas *t = top->snapshot;
        while (t != nullptr)
        {
            Tugas *tmp = t;
            t = t->next;
            delete tmp;
        }
        stack *tmpStack = top;
        top = top->top;
        delete tmpStack;
    }
}

// Linked List
void tambahTugas(string namaTugas, string namaMatkul, string deadline, bool status)
{
    push(undoTop, copyList(depan));

    Tugas *tugasBaru = new Tugas;
    tugasBaru->namaTugas = namaTugas;
    tugasBaru->namaMatkul = namaMatkul;
    tugasBaru->deadline = deadline;
    tugasBaru->status = status;
    tugasBaru->waktuMulai = system_clock::now();

    if (depan == NULL)
    {
        depan = tugasBaru;
    }

    else
    {
        Tugas *helper;
        helper = depan;
        while (helper->next != NULL)
        {
            helper = helper->next;
        }
        helper->next = tugasBaru;
    }
}

void hapusTugas(string namaTugas)
{

    push(undoTop, copyList(depan));

    if (depan == NULL)
    {
        cout << "Gaada Tugas Disini\n";
        return;
    }

    if (depan->namaTugas == namaTugas)
    {
        Tugas *temp;
        temp = depan;
        depan = depan->next;
        delete (temp);
        cout << "Tugas Dihapus..\n";
        return;
    }

    Tugas *helper;
    helper = depan;
    while (helper->next != nullptr && helper->next->namaTugas != namaTugas)

    {
        helper = helper->next;
    }

    if (helper->next != NULL)
    {
        Tugas *temp;
        temp = helper->next;
        helper->next = helper->next->next;
        delete (temp);
        cout << "Tugas:" << namaTugas << "Udah Kehapus";
        return;
    }
    else
    {
        cout << "Tugas itu gaada bro\n";
    }
}

void displayTugas()
{
    if (depan == NULL)
    {
        cout << "Tidak Ada Tugas.\n";
        return;
    }

    Tugas *helper = depan;
    cout << "Daftar Semua Tugas:\n";
    while (helper != nullptr)
    {
        cout << "----------------------------\n";
        cout << "Tugas       : " << helper->namaTugas << endl;
        cout << "Mata Kuliah : " << helper->namaMatkul << endl;
        cout << "Deadline    : " << helper->deadline << endl;
        cout << "Status      : " << (helper->status ? "selesai" : "belum selesai") << endl;
        displayTimeRemaining(*helper);
        helper = helper->next;
    }
}

void displayTugasMatkul(string namaMatkulCari)
{
    bool ditemukan = false;
    Tugas *helper = depan;

    cout << "Tugas Untuk Mata Kuliah : " << namaMatkulCari << endl;

    while (helper != NULL)
    {
        if (helper->namaMatkul == namaMatkulCari)
        {
            cout << "----------------------------\n";
            cout << "Tugas       : " << helper->namaTugas << endl;
            cout << "Deadline    : " << helper->deadline << endl;
            cout << "Status      : " << (helper->status ? "selesai" : "belum selesai") << endl;
            ditemukan = true;
        }
        helper = helper->next;
    }
    if (!ditemukan)
    {
        cout << "Tidak Ada Tugas Untuk Mata Kuliah \"" << namaMatkulCari << "\".\n";
    }
}

void sortbyDeadline()
{
    if (depan == NULL || depan->next == NULL)
        return;

    for (Tugas *a = depan; a != NULL; a = a->next)
    {
        for (Tugas *b = a->next; b != NULL; b = b->next)
        {
            if (a->deadline > b->deadline)
            {
                swap(a->namaTugas, b->namaTugas);
                swap(a->namaMatkul, b->namaMatkul);
                swap(a->deadline, b->deadline);
                swap(a->status, b->status);
            }
        }
    }
}

void ubahStatus(string tugas)
{
    bool ditemukan = false;
    Tugas *helper = depan;
    push(undoTop, copyList(depan));
    while (helper != NULL)
    {
        if (helper->namaTugas == tugas)
        {
            helper->status = !helper->status;
            cout << "Tugas    : " << helper->namaTugas << endl;
            cout << "Deadline : " << helper->deadline << endl;
            cout << "status   : " << (helper->status ? "selesai" : "belum selesai") << endl;
            ditemukan = true;
            break;
        }
        helper = helper->next;
    }
    if (!ditemukan)
    {
        cout << "\ntugas tidak di temukan" << tugas;
    }
}

void displayStatus(bool statusCari)
{
    bool ditemukan = false;
    Tugas *helper = depan;

    cout << "Tugas dengan Status : " << (statusCari ? "selesai" : "belum selesai") << endl;

    while (helper != NULL)
    {
        if (helper->status == statusCari)
        {
            cout << "Tugas    : " << helper->namaTugas << endl;
            cout << "Mata Kuliah : " << helper->namaMatkul << endl;
            cout << "Deadline : " << helper->deadline << endl;
            cout << "status   : " << (helper->status ? "selesai" : "belum selesai") << endl;
            ditemukan = true;
        }
        helper = helper->next;
    }
    if (!ditemukan)
    {
        cout << "tidak ada tugas dengan status tersebut/n";
    }
}

// File Handler
void safe()
{
    // Berarti kita harus loop ke setiap node nya, nah pas di looping node itu kita
    // Write aja data data dari node pake formatting ke txt file.

    Tugas *helper;
    helper = depan;

    fstream safe("holder.txt", ios::app);
    while (helper != NULL)
    {

        safe << "Nama Tugas: " << helper->namaTugas << endl;
        safe << "Mata Kuliah: " << helper->namaMatkul << endl;
        safe << "Deadline: " << helper->deadline << endl;
        safe << "Status: " << (helper->status ? "Selesai" : "Belum") << endl;
        safe << "--------------------------" << endl;
        helper = helper->next;
    }
    safe.close();
}
void load()
{

    // Berarti nih, bikin while buat open si file nya, sama bikin error handler.
    fstream load("holder.txt", ios::in);
    string text;

    // Error Handler
    if (!load.is_open())
    {
        cout << "Tidak Dapat Membuka File" << endl;
        return;
    }

    // Loop baca file
    string namaTugas, namaMatkul, deadline, seperatorline;
    string status;

    // Mengambil line setiap data yang ada di file
    while (getline(load, namaTugas))
    {
        getline(load, namaMatkul);
        getline(load, deadline);
        getline(load, status);
        getline(load, seperatorline);

        // Cek kalau string itu kosong atau engga
        if (namaTugas.empty() || namaMatkul.empty() || deadline.empty() || status.empty())
        {
            cout << "Warning: One of the lines is empty. Skipping this entry." << endl;
            continue; // Skip kalau emang ga kosong
        }

        bool statusConversion;
        // parsing data
        if (status.substr(8) == "Selesai")
        {
            statusConversion = true;
        }
        else
        {
            statusConversion = false;
        }
        // buat menghindari redudansi data
        Tugas *helper = depan;
        bool sudahAda = false;
        while (helper != nullptr)
        {
            if (helper->namaTugas == namaTugas.substr(12))
            {
                sudahAda = true;
                break;
            }
            helper = helper->next;
        }
        if (!sudahAda)
        {
            tambahTugas(namaTugas.substr(12), namaMatkul.substr(13), deadline.substr(10), statusConversion);
        }
        else
        {
            cout << "Tugas \"" << namaTugas << "\" sudah ada, Skipping....\n";
        }
    }
    load.close();
}

// Utilities
time_point<system_clock> parsingDeadline(string &deadline)
{
    // Jadi ini tuh ngebuat dulu struct dengan tipe data tm
    // Bawaan dari lib ctime nah (cek dokumentasi C++ lebih lanjut)
    // Udah gitu kita pake stringstream buat parsing
    // nah udah di parsing diubah lah jadi tanggal sama func get_time

    struct tm Time = {};
    stringstream parse(deadline);
    parse >> get_time(&Time, "%Y-%m-%d");

    // Error Handler
    if (parse.fail())
    {
        throw runtime_error("Failed to parse...");
    }

    // Sekarang Parsing dari struct tm kita ubah jadi time_t
    time_t parse2 = mktime(&Time);
    // Ubah dari time_t jadi time_point
    return system_clock::from_time_t(parse2);
}

bool displayTimeRemaining(Tugas &task)
{
    auto deadlineTime = parsingDeadline(task.deadline);
    auto now = system_clock::now();

    if (now < deadlineTime)
    {

        auto remaining = deadlineTime - now;
        auto jam = duration_cast<hours>(remaining).count();
        auto hari = jam / 24;
        jam %= 24;

        cout << "Sisa Waktu Buat Tugas " << task.namaTugas
             << " - Sisa: " << hari << " hari dan " << jam << " jam.\n";
        return true;
    }
    else
    {
        cout << "❌ Deadline tugas \"" << task.namaTugas << "\" sudah lewat!\n";
        return false;
    }
}

int main()
{
    int pilihan;
    string namaTugas, namaMatkul, deadline, detailBaru;
    bool status;

    cout << "Selamat Datang Di SantaiTapiJalan\n";
    cout << "Mau Numpuk Tugas Lagi Yeee?\n";

    do
    {
        cout << "\nMenu:\n";
        cout << "1. Tambah Tugas\n";
        cout << "2. Hapus Tugas\n";
        cout << "3. Ubah Status Tugas\n";
        cout << "4. Tampilkan Semua Tugas\n";
        cout << "5. Tampilkan Tugas Berdasarkan Mata Kuliah\n";
        cout << "6. Tampilkan Tugas Berdasarkan Status\n";
        cout << "7. Lihat Detail Tugas\n";
        cout << "8. Tambahkan Detail ke Tugas\n";
        cout << "9. Simpan Tugas ke File\n";
        cout << "10. Muat Tugas dari File\n";
        cout << "11. Urutkan Tugas Berdasarkan Deadline\n";
        cout << "12. Undo\n";
        cout << "13. Redo\n";
        cout << "14. Tambahkan Kategori Baru\n";
        cout << "15. Tampilkan Semua Kategori\n";
        cout << "16. Keluar\n";
        cout << "Pilih opsi (1-16): ";
        cin >> pilihan;

        switch (pilihan)
        {
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
            cout << "Masukkan Nama Tugas untuk mengubah status: ";
            cin.ignore();
            getline(cin, namaTugas);
            ubahStatus(namaTugas);
            break;

        case 4:
            displayTugas();
            break;

        case 5:
            cout << "Masukkan Nama Mata Kuliah: ";
            cin.ignore();
            getline(cin, namaMatkul);
            displayTugasMatkul(namaMatkul);
            break;

        case 6:
            cout << "Tampilkan tugas dengan status (1 untuk selesai, 0 untuk belum selesai): ";
            cin >> status;
            displayStatus(status);
            break;

        case 7:
            cout << "Masukkan Nama Tugas untuk melihat detail: ";
            cin.ignore();
            getline(cin, namaTugas);
            displayDetailTugas(namaTugas);
            break;

        case 8:
            cout << "Masukkan Nama Tugas untuk menambahkan detail: ";
            cin.ignore();
            getline(cin, namaTugas);
            cout << "Masukkan Detail Baru: ";
            getline(cin, detailBaru);
            tambahDetailTugas(namaTugas, detailBaru);
            break;

        case 9:
            safe();
            cout << "Tugas telah disimpan ke file.\n";
            break;

        case 10:
            load();
            break;

        case 11:
            sortbyDeadline();
            cout << "Tugas telah diurutkan berdasarkan deadline.\n";
            break;

        case 12:
            undo(undoTop, redoTop, depan);
            break;

        case 13:
            redo(undoTop, redoTop, depan);
            break;

        case 14:
            cout << "Masukkan Kategori Baru: ";
            cin.ignore();
            getline(cin, namaMatkul); // Menggunakan `namaMatkul` untuk kategori sementara
            tambahKategori(namaMatkul);
            break;

        case 15:
            displayKategori();
            break;

        case 16:
            cout << "Keluar dari aplikasi.\n";
            clearStack(undoTop);
            clearStack(redoTop);
            break;

        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            break;
        }
    } while (pilihan != 16);

    return 0;
}