#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DATA 50
#define FILE_NAME "PERPUSTAKAAN.txt"
#define FILE_HISTORY "HISTORY.txt"

/* ================= STRUCT ================= */
typedef struct {
    char namaBuku[50];
    char namaPeminjam[50];
    int kode;
    int NIM;
    int kondisiBuku;  // 1 tersedia, 0 dipinjam
    int tanggal;
} Buku;

Buku daftarBuku[MAX_DATA];
int jumlahData = 0;

/* ================= UTILITY ================= */
void bersihkanTampilan() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void bersihkanBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pause() {
    printf("\nTekan Enter untuk kembali...");
    bersihkanBuffer();
    getchar();
}

int getTanggalSekarang() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return (tm.tm_mday * 1000000) + ((tm.tm_mon + 1) * 10000) + (tm.tm_year + 1900);
}

void formatKodeBuku(int kode, char *hasil) {
    sprintf(hasil, "TE-%03d", kode);
}

/* ================= FILE BUKU ================= */
void simpanBuku() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) return;

    for (int i = 0; i < jumlahData; i++) {
        fprintf(fp, "%d|%s|%s|%d|%d|%d\n",
            daftarBuku[i].kode,
            daftarBuku[i].namaBuku,
            daftarBuku[i].namaPeminjam,
            daftarBuku[i].kondisiBuku,
            daftarBuku[i].tanggal,
            daftarBuku[i].NIM);
    }
    fclose(fp);
}

void bacaBuku() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return;

    jumlahData = 0;
    while (fscanf(fp, "%d|%[^|]|%[^|]|%d|%d|%d\n",
        &daftarBuku[jumlahData].kode,
        daftarBuku[jumlahData].namaBuku,
        daftarBuku[jumlahData].namaPeminjam,
        &daftarBuku[jumlahData].kondisiBuku,
        &daftarBuku[jumlahData].tanggal,
        &daftarBuku[jumlahData].NIM) == 6) {
        jumlahData++;
    }
    fclose(fp);
}

/* ================= HISTORY ================= */
void simpanHistory(const char *aksi, Buku b) {
    FILE *fp = fopen(FILE_HISTORY, "a");
    if (!fp) return;

    int t = getTanggalSekarang();
    int h = t / 1000000;
    int bl = (t / 10000) % 100;
    int th = t % 10000;

    fprintf(fp, "%s | %s | %s | NIM: %d | %02d-%02d-%04d\n",
            aksi, b.namaBuku, b.namaPeminjam, b.NIM, h, bl, th);
    fclose(fp);
}

void tampilkanHistory() {
    bersihkanTampilan();
    
    printf("=== HISTORY PEMINJAMAN ===\n\n");
    
    FILE *fp = fopen(FILE_HISTORY, "r");
    if (!fp) {
        printf("Belum ada history peminjaman.\n");
        pause();
        return;
    }

    char baris[200];
    int count = 0;
    while (fgets(baris, sizeof(baris), fp)) {
        printf("%s", baris);
        count++;
    }
    fclose(fp);
    
    if (count == 0) {
        printf("Belum ada history peminjaman.\n");
    }
    
    pause();
}

/* ================= DATA AWAL ================= */
void inisialisasiDataAwal() {
    char *judulBuku[] = {
        "Pemrograman C Dasar","Algoritma dan Struktur Data","Basis Data",
        "Sistem Operasi","Jaringan Komputer","Matematika Diskrit",
        "Kalkulus","Fisika Dasar","Kimia Dasar","Bahasa Indonesia",
        "Bahasa Inggris","Pendidikan Pancasila","Pendidikan Kewarganegaraan",
        "Pendidikan Agama","Kewirausahaan","Etika Profesi",
        "Metodologi Penelitian","Statistika","Grafika Komputer",
        "Kecerdasan Buatan","Data Mining","Machine Learning",
        "Web Programming","Mobile Programming","Database Management",
        "Software Engineering","Human Computer Interaction","Computer Security",
        "Network Security","Cloud Computing"
    };

    for (int i = 0; i < 30; i++) {
        daftarBuku[i].kode = i + 1;
        strcpy(daftarBuku[i].namaBuku, judulBuku[i]);
        strcpy(daftarBuku[i].namaPeminjam, "-");
        daftarBuku[i].kondisiBuku = 1;
        daftarBuku[i].NIM = 0;
        daftarBuku[i].tanggal = 0;
    }
    jumlahData = 30;
    simpanBuku();
}

/* ================= TAMPIL BUKU ================= */
void tampilkanDaftarBuku() {
    bersihkanTampilan();
    
    printf("=== DAFTAR BUKU ===\n\n");
    
    if (jumlahData == 0) {
        printf("Tidak ada buku dalam sistem.\n");
        pause();
        return;
    }
    
    printf("+--------+--------------------------------------+------------+----------------------+\n");
    printf("| KODE   | JUDUL BUKU                           | STATUS     | PEMINJAM             |\n");
    printf("+--------+--------------------------------------+------------+----------------------+\n");

    for (int i = 0; i < jumlahData; i++) {
        char kode[10];
        formatKodeBuku(i + 1, kode);
        printf("| %-6s | %-36s | %-10s | %-20s |\n",
            kode,
            daftarBuku[i].namaBuku,
            daftarBuku[i].kondisiBuku ? "Tersedia" : "Dipinjam",
            daftarBuku[i].kondisiBuku ? "-" : daftarBuku[i].namaPeminjam);
    }
    printf("+--------+--------------------------------------+------------+----------------------+\n");
    printf("\nTotal buku: %d\n", jumlahData);
    
    pause();
}

/* ================= PINJAM BUKU ================= */
void pinjamBuku() {
    int pilih[2], jml = 0;
    char nama[50];
    int NIM;

    bersihkanTampilan();
    printf("=== PEMINJAMAN BUKU ===\n\n");
    
    // Tampilkan buku yang tersedia
    printf("Buku yang tersedia untuk dipinjam:\n");
    printf("------------------------------\n");
    
    int tersedia = 0;
    for (int i = 0; i < jumlahData; i++) {
        if (daftarBuku[i].kondisiBuku) {
            printf("%d. %s\n", i + 1, daftarBuku[i].namaBuku);
            tersedia++;
        }
    }
    
    if (tersedia == 0) {
        printf("\nTidak ada buku yang tersedia untuk dipinjam.\n");
        pause();
        return;
    }
    
    printf("\n------------------------------\n");
    
    while (jml < 2) {
        int kode;
        printf("Masukkan kode buku ke-%d (0 untuk selesai): ", jml + 1);
        scanf("%d", &kode);
        
        if (kode == 0) {
            if (jml == 0) {
                printf("Tidak ada buku yang dipilih.\n");
                pause();
                return;
            }
            break;
        }

        int idx = kode - 1;
        if (idx < 0 || idx >= jumlahData) {
            printf("Kode buku tidak valid!\n");
        } else if (!daftarBuku[idx].kondisiBuku) {
            printf("Buku sedang dipinjam!\n");
        } else {
            // Cek agar tidak meminjam buku yang sama dua kali dalam satu sesi
            int sudahAda = 0;
            for(int j = 0; j < jml; j++) {
                if(pilih[j] == idx) sudahAda = 1;
            }
            
            if(sudahAda) {
                printf("Buku ini sudah ada di daftar pinjam!\n");
            } else {
                pilih[jml++] = idx;
                printf("Buku '%s' ditambahkan.\n", daftarBuku[idx].namaBuku);
            }
        }
        
        if (jml >= 2) break;
    }
    
    bersihkanBuffer();
     
    printf("\n=== DATA PEMINJAM ===\n");
    printf("Nama Peminjam: ");
    fgets(nama, 50, stdin);
    nama[strcspn(nama, "\n")] = 0;

    while (1) {
    printf("NIM: ");
    if (scanf("%d", &NIM) == 1) {
        bersihkanBuffer();
        break;              
    } else {
        printf("Input NIM tidak valid! Harus berupa angka.\n");
        bersihkanBuffer();
    }
    }

    // Proses Simpan Data
    int tgl = getTanggalSekarang();
    for (int i = 0; i < jml; i++) {
        int idx = pilih[i];
        daftarBuku[idx].kondisiBuku = 0;
        strcpy(daftarBuku[idx].namaPeminjam, nama);
        daftarBuku[idx].NIM = NIM;
        daftarBuku[idx].tanggal = tgl;
        simpanHistory("PINJAM", daftarBuku[idx]);
    }
    simpanBuku();

    // --- TAMPILAN STRUK ---
    bersihkanTampilan();
    int h = tgl / 1000000;
    int bl = (tgl / 10000) % 100;
    int th = tgl % 10000;

    printf("==========================================\n");
    printf("        STRUK PEMINJAMAN BUKU             \n");
    printf("        PERPUSTAKAAN TEKNIK               \n");
    printf("==========================================\n");
    printf(" Tanggal  : %02d-%02d-%04d\n", h, bl, th);
    printf(" Peminjam : %-20s\n", nama);
    printf(" NIM      : %-20d\n", NIM);
    printf("------------------------------------------\n");
    printf(" Daftar Buku:\n");
    
    for (int i = 0; i < jml; i++) {
        char kodeStr[10];
        formatKodeBuku(pilih[i] + 1, kodeStr);
        printf(" %d. [%-7s] %s\n", i + 1, kodeStr, daftarBuku[pilih[i]].namaBuku);
    }
    
    printf("------------------------------------------\n");
    printf(" Status   : BERHASIL DIPINJAM\n");
    printf(" Catatan  : Waktu peminjaman buku maksimal\n  2 minggu. Mohon dikembalikan tepat waktu.\n");
    printf("==========================================\n");
    printf("    TERIMA KASIH TELAH MEMINJAM BUKU      \n");
    printf("==========================================\n");

    pause();
}

/* ================= KEMBALI BUKU ================= */
void kembaliBuku() {
    int kode;
    bersihkanTampilan();

    printf("=== PENGEMBALIAN BUKU ===\n\n");
    
    // Tampilkan buku yang sedang dipinjam agar memudahkan user
    printf("Buku yang sedang dipinjam:\n");
    printf("------------------------------\n");
    
    int dipinjam = 0;
    for (int i = 0; i < jumlahData; i++) {
        if (!daftarBuku[i].kondisiBuku) {
            printf("%d. %s (Oleh: %s)\n", 
                   i + 1, daftarBuku[i].namaBuku, 
                   daftarBuku[i].namaPeminjam);
            dipinjam++;
        }
    }
    
    if (dipinjam == 0) {
        printf("\nTidak ada buku yang sedang dipinjam.\n");
        pause();
        return;
    }
    
    printf("\n------------------------------\n");
    printf("Masukkan kode buku yang dikembalikan: ");
    scanf("%d", &kode);
    
    int idx = kode - 1;

    if (idx < 0 || idx >= jumlahData) {
        printf("Kode buku tidak valid!\n");
    } else if (daftarBuku[idx].kondisiBuku) {
        printf("Buku ini tidak sedang dipinjam!\n");
    } else {
        // Simpan history sebelum data dihapus
        simpanHistory("KEMBALI", daftarBuku[idx]);
        
        // Simpan data sementara untuk struk
        char namaSmt[50];
        char judulSmt[50];
        int nimSmt = daftarBuku[idx].NIM;
        strcpy(namaSmt, daftarBuku[idx].namaPeminjam);
        strcpy(judulSmt, daftarBuku[idx].namaBuku);

        // Update status buku
        strcpy(daftarBuku[idx].namaPeminjam, "-");
        daftarBuku[idx].NIM = 0;
        daftarBuku[idx].kondisiBuku = 1;
        daftarBuku[idx].tanggal = 0;
        simpanBuku();

        // --- TAMPILAN STRUK PENGEMBALIAN ---
        int tgl = getTanggalSekarang();
        int h = tgl / 1000000;
        int bl = (tgl / 10000) % 100;
        int th = tgl % 10000;

        bersihkanTampilan();
        printf("==========================================\n");
        printf("        STRUK PENGEMBALIAN BUKU           \n");
        printf("        PERPUSTAKAAN TEKNIK               \n");
        printf("==========================================\n");
        printf(" Tanggal Kembali : %02d-%02d-%04d\n", h, bl, th);
        printf(" Nama Peminjam   : %-20s\n", namaSmt);
        printf(" NIM             : %-20d\n", nimSmt);
        printf("------------------------------------------\n");
        printf(" Judul Buku      : %s\n", judulSmt);
        printf(" Kode Buku       : TE-%03d\n", kode);
        printf("------------------------------------------\n");
        printf(" Status          : BERHASIL DIKEMBALIKAN\n");
        printf("==========================================\n");
        printf("    TERIMA KASIH TELAH MENGEMBALIKAN      \n");
        printf("==========================================\n");
    }
    
    pause();
}

/* ================= CRUD BUKU ================= */
void tambahBuku() {
    if (jumlahData >= MAX_DATA) {
        bersihkanTampilan();
        printf("=== TAMBAH BUKU ===\n\n");
        printf("Kapasitas buku penuh! Tidak dapat menambah buku baru.\n");
        pause();
        return;
    }

    bersihkanTampilan();
    printf("=== TAMBAH BUKU ===\n\n");
    
    printf("Judul Buku: ");
    fgets(daftarBuku[jumlahData].namaBuku, 50, stdin);
    daftarBuku[jumlahData].namaBuku[strcspn(daftarBuku[jumlahData].namaBuku, "\n")] = 0;

    daftarBuku[jumlahData].kode = jumlahData + 1;
    strcpy(daftarBuku[jumlahData].namaPeminjam, "-");
    daftarBuku[jumlahData].kondisiBuku = 1;
    daftarBuku[jumlahData].NIM = 0;
    daftarBuku[jumlahData].tanggal = 0;

    jumlahData++;
    simpanBuku();
    
    printf("\nBuku '%s' berhasil ditambahkan dengan kode TE-%03d!\n", 
           daftarBuku[jumlahData-1].namaBuku, jumlahData);
    pause();
}

void editBuku() {
    int kode;
    bersihkanTampilan();
    
    printf("=== EDIT BUKU ===\n\n");
    
    if (jumlahData == 0) {
        printf("Tidak ada buku dalam sistem.\n");
        pause();
        return;
    }
    
    printf("Masukkan kode buku yang akan diedit: ");
    scanf("%d", &kode);
    bersihkanBuffer();

    int idx = kode - 1;
    if (idx < 0 || idx >= jumlahData) {
        printf("Kode buku tidak valid!\n");
        pause();
        return;
    }
    
    if (!daftarBuku[idx].kondisiBuku) {
        printf("Buku sedang dipinjam! Tidak dapat diedit.\n");
        pause();
        return;
    }

    printf("\nJudul lama: %s\n", daftarBuku[idx].namaBuku);
    printf("Judul baru: ");
    
    fgets(daftarBuku[idx].namaBuku, 50, stdin);
    daftarBuku[idx].namaBuku[strcspn(daftarBuku[idx].namaBuku, "\n")] = 0;

    simpanBuku();
    
    printf("\nBuku berhasil diedit!\n");
    pause();
}

void hapusBuku() {
    int kode;
    bersihkanTampilan();
    
    printf("=== HAPUS BUKU ===\n\n");
    
    if (jumlahData == 0) {
        printf("Tidak ada buku dalam sistem.\n");
        pause();
        return;
    }
    
    printf("Masukkan kode buku yang akan dihapus: ");
    scanf("%d", &kode);

    int idx = kode - 1;
    if (idx < 0 || idx >= jumlahData) {
        printf("Kode buku tidak valid!\n");
        pause();
        return;
    }
    
    if (!daftarBuku[idx].kondisiBuku) {
        printf("Buku sedang dipinjam! Tidak dapat dihapus.\n");
        pause();
        return;
    }

    char judul[50];
    strcpy(judul, daftarBuku[idx].namaBuku);
    
    for (int i = idx; i < jumlahData - 1; i++) {
        daftarBuku[i] = daftarBuku[i + 1];
        daftarBuku[i].kode = i + 1;
    }
    
    jumlahData--;
    simpanBuku();
    
    printf("\nBuku '%s' berhasil dihapus!\n", judul);
    pause();
}

/* ================= MAIN ================= */
int main() {
    int pilih;
    
    // Load data dari file
    bacaBuku();
    if (jumlahData == 0) {
        inisialisasiDataAwal();
    }

    do {
        bersihkanTampilan();
        printf("=== MENU PERPUSTAKAAN ===\n\n");
        printf("1. Daftar Buku\n");
        printf("2. Pinjam Buku\n");
        printf("3. Kembalikan Buku\n");
        printf("4. History Peminjaman\n");
        printf("5. Tambah Buku\n");
        printf("6. Edit Buku\n");
        printf("7. Hapus Buku\n");
        printf("8. Keluar\n\n");
        printf("Pilih menu (1-8): ");
        
        if (scanf("%d", &pilih) != 1) {
            bersihkanBuffer();
            continue;
        }
        
        bersihkanBuffer();

        switch (pilih) {
            case 1: tampilkanDaftarBuku(); break;
            case 2: pinjamBuku(); break;
            case 3: kembaliBuku(); break;
            case 4: tampilkanHistory(); break;
            case 5: 
   
    {
        char password[5];  
        
        bersihkanTampilan();
        printf("=== TAMBAH BUKU ===\n\n");
        printf("Masukkan password admin: ");
        fgets(password, 50, stdin);
        password[strcspn(password, "\n")] = 0;  
        
        if (strcmp(password, "admin123") == 0) {
            tambahBuku();
        } else {
            printf("\nPassword salah!\n");
            pause();
        }
    }
    break;
            case 6:     {
        char password[50];  
        
        bersihkanTampilan();
        printf("=== EDIT BUKU ===\n\n");
        printf("Masukkan password admin: ");
        fgets(password, 50, stdin);
        password[strcspn(password, "\n")] = 0;  
        
        if (strcmp(password, "admin123") == 0) {
            editBuku();
        } else {
            printf("\nPassword salah!\n");
            pause();
        }
    }
    break;
            case 7:     {
        char password[50];  
        
        bersihkanTampilan();
        printf("=== HAPUS BUKU ===\n\n");
        printf("Masukkan password admin: ");
        fgets(password, 50, stdin);
        password[strcspn(password, "\n")] = 0;  
        
        if (strcmp(password, "admin123") == 0) {
            hapusBuku();
        } else {
            printf("\nPassword salah!\n");
            pause();
        }
    }
    break;
            case 8: 
                bersihkanTampilan();
                printf("=== KELUAR ===\n\n");
                printf("Terima kasih telah menggunakan sistem perpustakaan!\n");
                printf("Data telah disimpan.\n");
                break;
            default:
                printf("\nPilihan tidak valid! Silakan pilih 1-8.\n");
                pause();
                break;
        }
    } while (pilih != 8);

    return 0;
}
