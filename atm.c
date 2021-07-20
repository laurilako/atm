#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define BUFFER_SIZE 1000 //Luodaan maksimi puskurikoko myöhempää käyttöä varten (päivitetyn tilitiedoston kirjoitusta)

//Esitellään automaatin toiminta-funktiot
void paavalikko(char tilinumero[256]);
void otto();
void naytaSaldo();
// Stdin puskurin tyhjentämistä varten. ('Trailing Newline' ongelmaa varten).
void tyhjaa();
//Luodaan globaalit muuttujat jotka helpottavat ohjelman toimintaa funktioiden välisessä liikkumisessa.
int saldo = 0; 
char tilinumero[256];
int main(){
    FILE * fp;
    FILE * tfp;
    // Luodaan muuttujat joita tarvitaan tunnistautumisessa
    int syote = 0;
    char pin_oikea[256];
    char pin[256];
    char tnumero[256];
    int tarkistus=1;
    int c = 0; // laskuri sitä varten, että jos PIN syötetään kolme kertaa väärin niin automaatti suljetaan
    do{
        printf("Anna tilinumero: ");
        fgets(tnumero, 256, stdin);
        tnumero[strcspn(tnumero, "\n")] = 0;
        strcat(tnumero, ".acc");
        // Testataan löytyykö tilinumeron nimellä .acc tiedostoa.
        if((fp = fopen(tnumero, "r")) != NULL){
            fgets(pin_oikea, 256, fp); // Jos saadaan tilitiedosto auki, niin talletetaan oikea pinkoodi muuttujaan pin_oikea
            while(tarkistus != 0 && c < 3){
                printf("\nAnna PIN koodisi: "); // Kysytään käyttäjältä omaa pin koodia.
                fgets(pin, 256, stdin);
                pin[strcspn(pin, "\n")] = 0; // poistetaan newlinet pinkoodin lopusta
                pin_oikea[strcspn(pin_oikea, "\n")] = 0;
                    if ((tarkistus = strcmp(pin, pin_oikea)) == 0){ //Tarkistus palauttaa 0 jos pin koodi on oikein, muutoin se pysyy 1
                    fscanf(fp, "%d", &saldo); //Kaivetaan saldo globaaliin saldo muuttujaan .acc tiedostosta.
                    printf("PIN oikein!\n"); //Ilmoitetaan että pin oikein, ja avataan automaatti
                    printf("\nAvataan automaatti...\n");
                    strcpy(tilinumero, tnumero);
                    fclose(fp); // Suljetaan tilitiedosto josta luettiin tiedot ja tallennettiin ohjelmaan.
                    Sleep(3000);
                    paavalikko(tilinumero);}
                    else{
                        c++;
                        if(c != 3){ // Jos pin koodin syöttää väärin, kasvatetaan Virhe counteria (muuttuja c) ja ilmoitetaan siitä.
                            printf("PIN koodi vaarin, yrita uudestaan! yritys %d / 3", c);
                            }
                    }
            }
        }
        if(c != 3) // Jos ei löydy käyttäjätiedostoa koitetulla nimellä(numerolla), niin ilmoitetaan.
            printf("Tiliasi ei loytynyt!\n");
        if(c == 3){ // Pin syötetty 3 kertaa väärin, suljetaan automaatti.
            printf("Syotit PIN koodin 3 kertaa vaarin! Automaatti suljetaan...");
            Sleep(3000);
            exit(0);}

    }while (1);

return(0);
}

/*Käyttöliittymä - päävalikko jossa esitellään päätoiminnot ja niitä vastaavat numerot(valinta syöte)
Valinnasta riippuen, kutsutaan oikeaa toimintafunktiota. (Esim. valinta 1 -> kutsu otto funktiota),
jossa voidaan nostaa rahaa. */

void paavalikko(char tilinumero[256]){
    FILE * fp;
    FILE * tfp;
    //Määritellään valintamuuttuja (kokonaisluku jonka käyttäjä antaa)
    int valinta = 0, c = 0;
    //Määritellään puskuri ja tarvittava muuttuja saldolle joka suljettaessa päivitetään tilitiedostoon.
    char buffer[BUFFER_SIZE];
    char s[BUFFER_SIZE];
    printf("\n");
    printf("\n");
    printf("Valitse antamalla valintanumero (1-3)\nVoit lopettaa STOP-nappaimella (3)\n");
    printf("\n");
    printf("1 Otto\n");
    printf("2 Saldo\n");
    printf("3 STOP\n");
    printf("\n");
    printf("Valintasi: ");
    //Tähän tarkistus että valinta on kokonaisluku väliltä 1-3
    while(1){
        while(scanf("%d",&valinta)!=1 || valinta < 1 || valinta > 3){
            printf("Valinnan tulee olla kokonaisluku valilta 1-3!\n");
            tyhjaa();
            printf("Valintasi: ");
        }
        break;
    }
    switch (valinta)
    {
    case 1: // Kutsutaan oikea funktio valitusta toiminnasta riippuen
        otto();
        break;
    case 2:
        naytaSaldo();
        break;
    case 3:
        //Päivitetään muuttunut saldo tilitiedostoon. Käytetään apuna "temp.acc" tiedostoa johon kirjataan muut tiedot ja päivitetty saldo,
        //ja sitten poistetaan alkuperäinen tilitiedosto ja muutetaan uusi tiedosto uudeksi tilitiedostoksi.
        fp = fopen(tilinumero, "r");
        tfp = fopen("temp.acc", "w"); 
        sprintf(s, "%d", saldo);   
        while((fgets(buffer, BUFFER_SIZE, fp)) != NULL)
        {
            c++;
            if (c == 2)
                fputs(s, tfp);
            else
                fputs(buffer, tfp);
        }        
        //Vapautetaan molemmat tiedostotilat
        fclose(tfp);
        fclose(fp);
        //Poistetaan vanha tiedosto
        remove(tilinumero);
        //Muutetaan päivitetty tiedosto uudeksi tilitiedostoksi ja lopuksi suljetaan automaatti.
        rename("temp.acc", tilinumero);
        printf("Suljetaan automaatti...");
        Sleep(3000);
        exit(0);
        break;
    }
}

//Otto (eli nosto) funktion määrittely. Käyttäjä valitsee haluamansa summan syöttämällä taas vaihtoehdon.
void otto(){
    // Määritellään tarvittavat muuttujat.
    int summa=0;
    int valinta=0;
    int jaannos=0;
    int setelit[2];
    printf("Otto\n");
    printf("Valitse summa antamalla valintanumeron (1-7)\n");
    printf("1 20 euroa\n2 40 euroa\n3 60 euroa\n4 90 euroa\n5 140 euroa\n6 240 euroa\n7 Muu summa\n");
    printf("Valintasi: ");
    //Tarkistetaan, että valinta on "oikea" (luku välillä 1-7)
    while(1){
        while(scanf("%d",&valinta)!=1 || valinta < 1 || valinta > 7){
            printf("Valinnan tulee olla kokonaisluku valilta 1-7!\n");
            tyhjaa();
            printf("Valintasi: ");
        }
        break;
    }
    //valinnasta riippuen valitaan nostettava summa.
    switch (valinta)
    {
    case 1:
        summa = 20;
        break;
    case 2:
        summa = 40;
        break;
    case 3:
        summa = 60;
        break;
    case 4:
        summa = 90;
        break;
    case 5:
        summa = 140;
        break;
    case 6:
        summa = 240;
        break;
    case 7:
        printf("Anna muu summa. Summan täytyy olla mahdollinen 20e ja 50e seteleilla, maksimissaan 1000e\n");
        printf("Summa: ");
        while(1){ 
            //Tarkistetaan, että annettu summa on laillinen (Aikaisemman kotitehtävän määreiden mukaisesti. 20e tai 40e, tai sitä suurempi 10 euron välein max 1000e)
            while(scanf("%d", &summa)!=1 || summa % 10 != 0 || (summa < 40 && summa != 20) || summa > 1000){
                printf("Virheellinen summa!\n");
                tyhjaa();
                printf("Anna summa: ");
                }
            break;
        }
    }
    
    if(saldo - summa <= 0){ // Tarkistetaan, että saldo ei pääse miinukselle. Jos nostettava summa on liian iso, käynnistetään otto-valikko uudestaan.
        printf("Et voi nostaa summaa joka on suurempi kuin saldosi (%d euroa)!\n", saldo);
        otto();}

    // Jos saldo pysyy plussan puolella, hoidetaan sen vähennys ja käyttäjän valinnan mukaan tulostetaan tiedot nostotoimenpiteestä.
    saldo = saldo-summa;
    printf("Haluatko tiedot?\n");
    printf("1 Naytolle\n2 Kuitille\n3 Ei kiitos (palaa paavalikkoon)\n");
    printf("Valintasi: ");
    while(1){
        while(scanf("%d",&valinta)!=1 || valinta < 1 || valinta > 3){
            printf("Valinnan tulee olla kokonaisluku valilta 1-3!\n");
            tyhjaa();
            printf("Valintasi: ");
        }
        break;}
    switch (valinta)
        {
        case 1:
            printf("Nostotoimenpide: %d euroa\n", summa);
            printf("Tilisi saldo noston jalkeen: %d euroa\n", saldo);
            printf("Rahat ovat tulossa.\nPalataan paavalikkoon 3 sekunnin paasta.");
            Sleep(3000);
            paavalikko(tilinumero);
        case 2:
            printf("Rahat ja tiedot kuitilla ovat tulossa.\nPalataan paavalikkoon 3 sekunnin paasta.");
            Sleep(3000);
            paavalikko(tilinumero);
        case 3:
            printf("Palataan paavalikkoon 3 sekunnin paasta.");
            Sleep(3000);
            paavalikko(tilinumero);
        }
}

//Tämä funktio tulostaa käyttäjän saldon, ja odottaa, että käyttäjää syöttää 1,
//jolloin siirrytään takaisin päävalikkoon.
void naytaSaldo(){
    int syote;
    printf("Tilisi saldo    %d euroa\n", saldo);
    printf("Syota 1 lopettaaksesi (siirry paavalikkoon): ");
    while(scanf("%d", &syote)!=1 || syote != 1){
        printf("\nSyota 1 lopettaaksesi (siirry paavalikkoon): ");
        tyhjaa();
    }
    paavalikko(tilinumero);
}

//Stdin puskurin tyhjennysfunktio, jos satutaan tarvitsemaan.
void tyhjaa(){
    while(getc(stdin)!='\n');  
}