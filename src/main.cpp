#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// LIST LIST ERROR YANG HARUS DI FIX
// TODO:
// 1. Nambahin 3 Struktur data lagi
// 2. Filter Tugas Pake status
// 3. Detail tugas
// 4. Undo Dan Redo.
// 5. Masukkin yang fungsi load ke linked list
// 6. Coba gimana caranya biar file txt ngga re-write

struct Tugas
{
    string namaTugas;
    string namaMatkul;
    string deadline; // Format "YYYY-MM-DD"
    bool status;
    time_point<system_clock> waktuMulai;
    Tugas *next = nullptr;
};

// Algoritma buat stack 
/*
1. Copy Dulu state linked list sebelumnya
2. Push state ke dalam stack
3. Pop kalo emang udah dibutuhin 
4. Baru bikin operasi 


*/
struct stack
{

};

Tugas *depan = NULL;

void tambahTugas(string namaTugas, string namaMatkul, string deadline, bool status)
{
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
    // Case 1: Kalo gaada node
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
    while (helper->next->namaTugas != namaTugas && helper->next != NULL)
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
    Tugas* helper = depan;

    while (helper != NULL)
    {
        if (helper->namaTugas == tugas){
            helper->status = !helper->status;
            cout<<"Tugas    : "<<helper->namaTugas<<endl;
            cout<<"Deadline : "<<helper->deadline<<endl;
            cout<<"status   : "<<(helper->status ? "selesai" : "belum selesai")<< endl;
            ditemukan = true;
            break;
        }
        helper = helper->next;
        
    }
    if (!ditemukan){
        cout<<"\ntugas tidak di temukan"<<tugas;
    }
}

void displayStatus(bool statusCari)
{
    bool ditemukan = false;
    Tugas* helper = depan;

    cout<<"Tugas dengan Status : "<<(statusCari ? "selesai" : "belum selesai")<<endl;

    while (helper != NULL){
        if(helper->status == statusCari){
            cout<<"Tugas    : "<<helper->namaTugas<<endl;
            cout << "Mata Kuliah : " << helper->namaMatkul << endl;
            cout<<"Deadline : "<<helper->deadline<<endl;
            cout<<"status   : "<<(helper->status ? "selesai" : "belum selesai")<< endl;
            ditemukan = true;
        }
        helper = helper->next;
    }
    if (!ditemukan){
        cout<<"tidak ada tugas dengan status tersebut/n";
    }
}


void safe()
{
    // Berarti kita harus loop ke setiap node nya, nah pas di looping node itu kita
    // Write aja data data dari node pake formatting ke txt file.

    Tugas *helper;
    helper = depan;

    fstream safe("holder.txt", ios::out);
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
    while (getline(load, text))
    {
        cout << text << endl;
    }

    load.close();
}

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
        cout << "7. Lihat Detail Tugas\n";
        cout << "8. Simpan Tugas ke File\n";
        cout << "9. Muat Tugas dari File\n";
        cout << "10. Urutkan Tugas Berdasarkan Deadline\n";
        cout << "11. Undo\n";
        cout << "12. Redo\n";
        cout << "13. Keluar\n";
        cout << "Pilih opsi (1-13): ";
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
            // Tambahkan fungsi untuk menampilkan detail tugas
            break;
    
        case 8:
            safe();
            cout << "Tugas telah disimpan ke file.\n";
            break;
    
        case 9:
            load();
            break;
    
        case 10:
            sortbyDeadline();
            cout << "Tugas telah diurutkan berdasarkan deadline.\n";
            break;
    
        case 11:
            // Tambahkan fungsi undo
            break;
    
        case 12:
            // Tambahkan fungsi redo
            break;
    
        case 13:
            cout << "Keluar dari aplikasi.\n";
            break;
    
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            break;
        }
    
    } while (pilihan != 13);
}
