#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace std::chrono;

struct Tugas
{
    string namaTugas;
    string namaMatkul;
    string deadline; // Format "YYYY-MM-DD"
    bool status = false;
    time_point<system_clock> waktuMulai;
    Tugas *next = nullptr;
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

    Tugas *helper;
    helper = depan;
    while (helper->next != NULL)
    {
        helper = helper->next;
    }
    helper->next = tugasBaru;
}

void hapusTugas(string namaTugas)
{
    if (depan == NULL)
    {
        return;
    }
    if (depan->namaTugas == namaTugas)
    {
        Tugas *temp;
        temp = depan;
        depan = depan->next;
        delete (temp);
    }
    Tugas *helper;
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
    }
}
void displayTugas()
{
    if (depan == NULL)
    {
        cout << "Tidak Ada Tugas./n";
        return;
    }

    Tugas *helper = depan;
    cout << "Daftar Semua Tugas:/n";
    while (helper != NULL)
    {
        cout << "----------------------------/n";
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
            cout << "----------------------------/n";
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
    // Struct Tugas
    // Buat Temp nya dimana dah?
    Tugas *helper;
    helper = depan;

    Tugas *temp;
    temp = helper;

    // Kalau tugas itu depan
    if (helper == depan)
    {
        depan = helper->next;
        temp = helper;
        delete (temp);
    }
    // Mencari nama tugas sesuai dengan linked list yang ada
    while (helper->next != NULL)
    {
        helper = helper->next;
    }

    if (helper->namaTugas == tugas)
    {
        helper->status = true;
        temp = helper;
        delete (temp);
    }
}

void safe()
{
    // Berarti kita harus loop ke setiap node nya, nah pas di looping node itu kita
    // Write aja data data dari node pake formatting ke txt file.

    Tugas *helper;
    helper = depan;

    fstream safe("holder.txt", ios::out);
    while (helper->next != NULL)
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
        // HIDUP JOKOWI!!!!!!!
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

    cout << "Selamat Datang Di SantaiTapiJalan" << endl;
    cout << "Mau numpuk tugas lagi ya?" << endl;

    switch (pilihan)
    {
    case 1:
    }
}
