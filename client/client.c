//Masih logika sama error handling gaes

//BUAT CHECK ROOT ATAU BUKAN
int checkSudo() {
    uid_t uid=getuid(), euid=geteuid();
    if(uid != 0) {
     return 0; // Login as user
    } else {
     return 1; //Login as root
    }
}

//Check login
if (strcmp(argv[1], "-u") == 0 && argv[2] != 0) {
       if (strcmp(argv[3], "-p") == 0 && argv[4] != 0) {
             if (user & pass ada) {
                    //login
             } else {
                    // user / password salah
             }
       } else {
              //echo "Butuh password"
       }
} else {
       //echo "Welcome, to use this program add argument -u for user and -p for password"
}

//Setelah login
