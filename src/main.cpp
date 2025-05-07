#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// LIST LIST ERROR YANG HARUS DI FIX
// TODO:
//  Bikin Detail Tugas
//  Sebisa Mungkin bikin error handling buat semua fungsi

// struct buat data data
struct Tugas
{
    string namaTugas;
    string namaMatkul;
    string deadline; // Format "YYYY-MM-DD"
    bool status;
    string detilTugas;
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
Tugas *tugasArray[100];
int jumlahTugas = 0;

// Array buat menghitung persentase
void konversiArray()
{
    jumlahTugas = 0;
    Tugas *helper = depan;

    while (helper != nullptr && jumlahTugas < 100)
    {
        tugasArray[jumlahTugas++] = helper;
        helper = helper->next;
    }
}

void menunjukkanPersen()
{
    konversiArray();
    if (jumlahTugas == 0)
    {
        cout << "Senang Kali Hidup Mu Wak" << endl;
        return;
    }

    int selesai = 0;
    for (int i = 0; i < jumlahTugas; i++)
    {
        if (tugasArray[i]->status)
        {
            selesai++;
        }
    }
    cout << "Progress   :" << (selesai * 100 / jumlahTugas) << "% selesai.. \n";
}

// Stack
Tugas *copyList(Tugas *task)
{
    if (task == nullptr)
    {
        cout << "Gaada tugas masbro" << endl;
        return nullptr;
    }
    // Ini tuh kopas kepala dari linked list satu ke linked list baru
    Tugas *pbaru = new Tugas;
    pbaru->namaTugas = task->namaTugas;
    pbaru->namaMatkul = task->namaMatkul;
    pbaru->deadline = task->deadline;
    pbaru->status = task->status;
    pbaru->waktuMulai = task->waktuMulai;
    pbaru->detilTugas = task->detilTugas;
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
        baru->detilTugas = helper->detilTugas;
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
    cout << "Berhasil Redo..\n";
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
    tugasBaru->detilTugas = "Tidak Ada detail";
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

time_point<system_clock> parsingDeadline(string &deadline);
bool displayTimeRemaining(Tugas &task);

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
        cout << "Detail      : " << helper->detilTugas << endl;
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
void nambahDetailTugas(string namaTugas, string detilTugas)
{
    push(undoTop, copyList(depan));

    Tugas *helper = depan;
    bool ditemukan = false;

    // Cari tugas berdasarkan nama
    while (helper != nullptr)
    {
        if (helper->namaTugas == namaTugas)
        {
            // Jika tugas ditemukan, tambahkan detail
            helper->detilTugas = detilTugas;
            cout << "Nama Tugas :" << helper->namaTugas << endl;
            cout << "Detail     :" << helper->detilTugas << endl;
            ditemukan = true;
            break;
        }
        helper = helper->next;
    }

    if (!ditemukan)
    {
        cout << "Tugas dengan nama \"" << namaTugas << "\" tidak ditemukan.\n";
    }
}

// File Handler
void safe()
{
    // Berarti kita harus loop ke setiap node nya, nah pas di looping node itu kita
    // Write aja data data dari node pake formatting ke txt file.

    Tugas *helper;
    helper = depan;
    fstream safe("holder.txt", ios::out);
    if (!safe.is_open())
    {
        cout << "Gagal membuka file untuk menyimpan data.\n";
        return;
    }
    while (helper != NULL)
    {

        safe << "Nama Tugas: " << helper->namaTugas << endl;
        safe << "Mata Kuliah: " << helper->namaMatkul << endl;
        safe << "Deadline: " << helper->deadline << endl;
        safe << "Status: " << (helper->status ? "Selesai" : "Belum") << endl;
        safe << "Detail: " << (helper->detilTugas.empty() ? "Tidak ada detail" : helper->detilTugas) << endl;
        safe << "--------------------------" << endl;
        helper = helper->next;
    }
    cout << "Data berhasil disimpan ke file.\n";
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
    string namaTugas, namaMatkul, deadline, seperatorline, detailTugas;
    string status;

    // Mengambil line setiap data yang ada di file
    while (getline(load, namaTugas))
    {
        getline(load, namaMatkul);
        getline(load, deadline);
        getline(load, status);
        getline(load, detailTugas);
        getline(load, seperatorline);

        // Cek kalau string itu kosong atau engga
        if (namaTugas.empty() || namaMatkul.empty() || deadline.empty() || status.empty() || detailTugas.empty())
        {
            cout << "Warning: One of the lines is empty. Skipping this entry." << endl;
            continue; // Skip kalau emang ga kosong
        }
        string namaTugasValue = namaTugas.substr(12);   // Menghapus "Nama Tugas: "
        string namaMatkulValue = namaMatkul.substr(13); // Menghapus "Mata Kuliah: "
        string deadlineValue = deadline.substr(10);     // Menghapus "Deadline: "
        string statusValue = status.substr(8);          // Menghapus "Status: "
        string detailValue = detailTugas.substr(8);     // Menghapus "Detail: "
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
            Tugas *tugasBaru = new Tugas;
            tugasBaru->namaTugas = namaTugasValue;
            tugasBaru->namaMatkul = namaMatkulValue;
            tugasBaru->deadline = deadlineValue;
            tugasBaru->status = statusConversion;
            tugasBaru->detilTugas = (detailValue == "Tidak ada detail" ? "Tidak ada detail" : detailValue);
            tugasBaru->next = nullptr;

            if (depan == nullptr)
            {
                depan = tugasBaru;
            }
            else
            {
                Tugas *helper = depan;
                while (helper->next != nullptr)
                {
                    helper = helper->next;
                }
                helper->next = tugasBaru;
            }
        }
        else
        {
            cout << "Tugas \"" << namaTugasValue << "\" sudah ada. Melewati entri ini.\n";
        }
    }
    cout << "Data Sudah Masuk" << endl;
    load.close();
}

// utility
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

// Queue
void sisipkanPrioritas(Tugas *&queuePrioritas, Tugas *tugasBaru)
{

    auto deadlineBaru = parsingDeadline(tugasBaru->deadline);

    if (queuePrioritas == nullptr || parsingDeadline(tugasBaru->deadline) < parsingDeadline(queuePrioritas->deadline))
    {
        tugasBaru->next = queuePrioritas;
        queuePrioritas = tugasBaru;
    }
    else
    {
        Tugas *helper = queuePrioritas;
        while (helper->next != nullptr && parsingDeadline(tugasBaru->deadline) >= parsingDeadline(helper->next->deadline))
        {
            helper = helper->next;
        }
        tugasBaru->next = helper->next;
        helper->next = tugasBaru;
    }
}
void autoPop(Tugas *&queuePrioritas)
{
    auto now = system_clock::now();

    while (queuePrioritas != nullptr && parsingDeadline(queuePrioritas->deadline) < now)
    {
        cout << "❌ Tugas \"" << queuePrioritas->namaTugas << "\" telah melewati deadline dan dihapus dari queue.\n";
        Tugas *temp = queuePrioritas;
        queuePrioritas = queuePrioritas->next;
        delete temp;
    }
}
void tampilkanQueueprioritas()
{
    Tugas *queuePrioritas = nullptr;
    Tugas *helper = depan;

    while (helper != nullptr)
    {
        Tugas *salin = new Tugas;
        *salin = *helper;
        salin->next = nullptr;

        sisipkanPrioritas(queuePrioritas, salin);

        helper = helper->next;
    }
    // autopop
    autoPop(queuePrioritas);
    // tampilkan
    Tugas *temp = queuePrioritas;
    while (temp != nullptr)
    {
        cout << "----------------------------\n";
        cout << "Tugas    : " << temp->namaTugas << endl;
        cout << "Mata Kuliah : " << temp->namaMatkul << endl;
        cout << "Deadline : " << temp->deadline << endl;
        cout << "status   : " << (temp->status ? "selesai" : "belum selesai") << endl;
        displayTimeRemaining(*temp);
        temp = temp->next;
    }
    // bebaskan memori
    while (queuePrioritas != nullptr)
    {
        Tugas *hapus = queuePrioritas;
        queuePrioritas = queuePrioritas->next;
        delete hapus;
    }
}

int main()
{
    int pilihan;
    string namaTugas, namaMatkul, deadline;
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
        cout << "7. Input Detail Tugas\n";
        cout << "8. Simpan Tugas ke File\n";
        cout << "9. Muat Tugas dari File\n";
        cout << "10. Hardcore Mode\n"; // Tugas dimasukkan kedalam queue dan kalo udah diluar waktu itu bakal pop sendiri;
        cout << "11. Undo\n";
        cout << "12. Redo\n";
        cout << "13. Statistik\n";
        cout << "14. Keluar\n";
        cout << "Pilih opsi (1-14): ";
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
            displayTugas();
            cout << "Masukkan Nama Tugas yang ingin ditambahkan detail: ";
            cin.ignore(); // Membersihkan buffer input
            getline(cin, namaTugas);
            cout << "Masukkan Detail Tugas: ";
            getline(cin, deadline); // Gunakan variabel sementara seperti `deadline` untuk detail
            nambahDetailTugas(namaTugas, deadline);
            break;

        case 8:
            safe();
            cout << "Tugas telah disimpan ke file.\n";
            break;

        case 9:
            load();
            break;

        case 10:
            cout << "Hardcore Mode: Tugas akan otomatis dihapus jika melewati deadline.\n";
            tampilkanQueueprioritas();
            break;

        case 11:
            undo(undoTop, redoTop, depan);
            break;

        case 12:
            redo(undoTop, redoTop, depan);
            break;

        case 13:
            menunjukkanPersen();
            break;

        case 14:
            cout << "Keluar dari aplikasi.\n";
            clearStack(undoTop);
            clearStack(redoTop);
            break;

        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            break;
        }
    } while (pilihan != 14);
}