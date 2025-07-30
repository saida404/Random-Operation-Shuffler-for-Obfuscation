#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <ctime>
using namespace std;

/*tema 1*/

/*Ideja je iz ulaznog stringa razdvojiti ulaze operacije na komponente koje se cuvaju u strukturi Operacija, zatim kreirati cvorove i graf,
tako da su dva cvora povezana ako je ulazna operacija prvog cvora  izlazna operacija drugog cvora i tada prvi cvor zavisi od drugog,
nakon kreiranja grafa radim topolosko sortiranje, tako da pratim koji cvorovi vise nemaju zavisnosti od drugih, stavim ih u vektor i zatim koristeci random generisem
indeks iz vektora i tako nasumicno cvorove ubacam u red.
FOR petlja se gleda kao jedan cvor, a njeni izlazi i ulazi su ustvari ulazi i izlazi svih operacija koje se nalaze unutar nje*/


//Funkcija za uklanjanje whitespace-a
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == string::npos || last == string::npos)
        return "";
    return str.substr(first, (last - first + 1));
}

/*Funkcija koja iz stringa izdvaja podstringove koji su odvojeni granicnikom uklanjajuci bespotrebni whitepsace */
vector<string> podijeli(const string& s, char granicnik) {
    vector<string> rezultat;
    string z;

    for (char znak : s) {
        if (znak == granicnik) {
            if (!z.empty()) {
                rezultat.push_back(trim(z));
                z.clear();
            }
        } else {
            z += znak;
        }
    }

    if (!z.empty()) {
        rezultat.push_back(trim(z));
    }

    return rezultat;
}

/*Struktura u koju spasavamo operacije iz stringa, imamo vektor izlaznih i ulaznih parametar, te citavu operaciju (kod)*/
struct Operacija {
    vector<string> izlazi;
    vector<string> ulazi;
    string kod;

     Operacija(const vector<string>& izlazi, const vector<string>& ulazi, const string& kod)
        : izlazi(izlazi), ulazi(ulazi), kod(kod) {}
};

ostream& operator<<(ostream& os, const Operacija& op) {
    os << op.kod << endl;
    return os;
}


/* Cvor grafa sadrzi Operaciju i susjede, te ulazni stepen.*/
struct Cvor {
    Operacija op;
    vector<Cvor*> susjedi;
    int ulazniStepen;  // broj operacija koje za izlaz imaju neki od ulaza tekuce operacije

    Cvor(const Operacija& operacija) : op(operacija), ulazniStepen(0) {}

};

/* Struktura graf i osnovne operacije*/
struct Graf {
    vector<Cvor*> cvorovi;

    // dodavanje operacije u graf
    void dodajOperaciju(const Operacija& op) {
        cvorovi.push_back(new Cvor(op));
    }

    // dodavanje susjeda izmedju operacija
    void dodajSusjeda(int indeks1, int indeks2) {
        if (indeks1 >= 0 && indeks1 < cvorovi.size() && indeks2 >= 0 && indeks2 < cvorovi.size()) {
            cvorovi[indeks1]->susjedi.push_back(cvorovi[indeks2]);
        }
    }

    // uklanjanje susjeda
    void ukloniSusjeda(int indeks1, int indeks2) {
        if (indeks1 >= 0 && indeks1 < cvorovi.size()) {
            auto& susjedi = cvorovi[indeks1]->susjedi;
            susjedi.erase(remove(susjedi.begin(), susjedi.end(), cvorovi[indeks2]), susjedi.end());
        }
    }

    // iispis grafa
  void ispis() const {
    for (int i = 0; i < cvorovi.size(); ++i) {
        cout << "Operacija " << i + 1 << ": " << cvorovi[i]->op << "\n";
        cout << "Susjedi: ";
        for (Cvor* susjed : cvorovi[i]->susjedi) {

            int indeks = find(cvorovi.begin(), cvorovi.end(), susjed) - cvorovi.begin();
            cout << indeks + 1 << " ";
        }
        cout << "\nZavisi od: " << cvorovi[i]->ulazniStepen << "\n";
        cout << "---------------------------\n";
    }
}
    // Ddestruktor
    ~Graf() {
        for (Cvor* c : cvorovi) {
            delete c;
        }
    }

    /*Funkcija za kreiranje grafa za date operacije*/
   void kreirajGraf(const vector<Operacija>& operacije) {

    // dodamo sve operacije u graf
    for (const Operacija& op : operacije) {
        dodajOperaciju(op);
    }

    // kreiramo zavisnosti izmedju operacija i racunamo ulazni stepen
    for (int i = 0; i < operacije.size(); ++i) {
        for (int j = 0; j < operacije.size(); ++j) {
            if (i != j) {
                // ako operacija i ima za ulaz varijablu koju generise operacija j, povezi ih
                for (const string& izlaz : operacije[j].izlazi) {
                    for (const string& ulaz : operacije[i].ulazi) {
                        if (izlaz == ulaz) {
                            dodajSusjeda(j, i);     // dodaj j kao susjeda i (povezuje se u obrnutom redoslijedu)
                            cvorovi[i]->ulazniStepen++; // povecj ulazni stepen cvora koji ima zavisnost
                            break;
                        }
                    }
                }
            }
        }
    }
}


};

// top sort operacija
vector<int> topoloskoSortiranje(Graf& graf) {
    srand(time(0));

    vector<int> topoloskiRed;
    vector<Cvor*> vektorSaStepenom0; //ovdje cuvamo sve cvorove koji u istom trenutku imaju stepen 0, kako bi ih poslije random mogli ubaciti u red

    queue<Cvor*> q;

    // dodajemo vcorove sa ulaznim stepenom 0 u vektor
    for (Cvor* cvor : graf.cvorovi) {
        if (cvor->ulazniStepen == 0) {

            vektorSaStepenom0.push_back(cvor);
        }
    }

    // nasumicno dodajemo cvorove sa ulaznim stepenom 0 u red
    while (!vektorSaStepenom0.empty()) {
        int randIndex = rand() % vektorSaStepenom0.size();   // generisemo slucajni indeks
       // cout<<"GENERISAO SAM INDEKS "<<randIndex<<endl;

        Cvor* cvor = vektorSaStepenom0[randIndex];
        vektorSaStepenom0.erase(vektorSaStepenom0.begin() + randIndex);  // brisemo iz vektora cvor koji dodajemo u red

       // cout<<"DODAJEM "<<cvor->op.kod<<endl;
       q.push(cvor);
    }


    while (!q.empty()) {
        Cvor* trenutniCvor = q.front();
        q.pop();
      //  cout<<"Trenutni: "<<trenutniCvor->op.kod<<endl;

        // dodajemo trenutni cvor u red
        int indeksTrenutnog = find(graf.cvorovi.begin(), graf.cvorovi.end(), trenutniCvor) - graf.cvorovi.begin();
        topoloskiRed.push_back(indeksTrenutnog);

        //prolazimo kroz susjede trenutnog cvora
        for (Cvor* susjed : trenutniCvor->susjedi) {
            susjed->ulazniStepen--;  // Smanjujemo ulazni stepen susjeda

            if (susjed->ulazniStepen == 0) {
                vektorSaStepenom0.push_back(susjed);  // dodajemo one susjede koji ce nakon smanjena zavisnosti imati stepen 0 u vektor
            }
        }


        // nasumicno dodajemo cvorove iz vektora u red
        while (!vektorSaStepenom0.empty()) {


            int randIndex = rand() % vektorSaStepenom0.size();
            Cvor* cvor = vektorSaStepenom0[randIndex];
            vektorSaStepenom0.erase(vektorSaStepenom0.begin() + randIndex);
            q.push(cvor);

        }
    }

    //provjera da li postoji ciklus u grafu
    if (topoloskiRed.size() != graf.cvorovi.size()) {
        cout << "Graf sadrzi ciklus! Topolosko sortiranje nije moguce."<<endl;
        return {};
    }

    return topoloskiRed;
}


//Funkcija koja prima liniju i zardvaja je na komponente operacije
Operacija razdvojiOperaciju(const string& linija) {
    vector<string> izlazi, ulazi;
    string kod = linija;

    //trazim poziciju '='
    int jednakoPoz = linija.find('=');
    if (jednakoPoz != string::npos) {
        string izlaziStr = linija.substr(0, jednakoPoz);
        izlazi = podijeli(izlaziStr, ',');


    }

    //razdvajanje ulaza unutar zagrada
    int otvorenaZagrada = kod.find('(');
    int zatvorenaZagrada = kod.find(')');
    string ulaziStr;
    if (otvorenaZagrada != string::npos && zatvorenaZagrada != string::npos) {
        ulaziStr = kod.substr(otvorenaZagrada + 1, zatvorenaZagrada - otvorenaZagrada - 1);
    }

    ulazi = podijeli(ulaziStr, ',');  //razdvajanje ulaza po zarezima

    //vracamo Operaciju sa izlazima, ulazima i kodom
    return {izlazi, ulazi, kod};
}


/*Funkcija koja uzima for blok, */
void obradiForPetlju(vector<string>& trenutneOperacije, vector<Operacija>& rezultat) {
    string kodPetlje = "FOR\n";
    vector<string> sviUlazi;
    vector<string> sviIzlazi;
    vector<Operacija> vektorOpUnutar;

    for (const string& operacija : trenutneOperacije) {
        //ignorisemo end
        if (operacija.find("END") != string::npos) continue;

        //razdvodjimo operaciije i dodamo u vektor operacija
        Operacija op = razdvojiOperaciju(operacija);
        vektorOpUnutar.push_back(op);

        //ulazi i izlazi petlje
        for (const string& izlaz : op.izlazi) {
            if (find(sviIzlazi.begin(), sviIzlazi.end(), izlaz) == sviIzlazi.end()) {
                sviIzlazi.push_back(izlaz);
            }
        }
        for (const string& ulaz : op.ulazi) {
            if (find(sviUlazi.begin(), sviUlazi.end(), ulaz) == sviUlazi.end()) {
                sviUlazi.push_back(ulaz);
            }
        }
    }

    //posebno graf za operacije unutar FOR petlje kako bi se mogle operacije unutar petlje mijesati
    Graf grafFor;
    grafFor.kreirajGraf(vektorOpUnutar);
    auto sortirane = topoloskoSortiranje(grafFor);

    //kod for petlje
    for (int i : sortirane) {
        Operacija oper = grafFor.cvorovi[i]->op;
        kodPetlje += oper.kod + "\n";
    }

    kodPetlje += "END";

    //dodaj FOR petlju kao jednu operaciju u rezultat
    Operacija opPetlja = {sviIzlazi, sviUlazi, kodPetlje};
    rezultat.push_back(opPetlja);

    trenutneOperacije.clear();
}

/*Funkcija koja iz stringa odvaja operacije (ulaze,izlaze i kod),te ih dodaje  u vektor operacija kao objekat strukture Operacija*/
vector<Operacija> razdvojiOperacije(const string& operacije) {
    vector<Operacija> rezultat;
    vector<string> linije = podijeli(operacije, '\n');

    vector<string> trenutneOperacije;  // Linije unutar FOR petlje
    bool unutarForPetlje = false;

    for (const string& linija : linije) {
        if (linija.empty()) continue;

        if (linija.find("FOR") != string::npos) {
            unutarForPetlje = true;
            continue;
        }

        if (unutarForPetlje) {
            trenutneOperacije.push_back(linija);

            if (linija.find("END") != string::npos) {
                unutarForPetlje = false;
                obradiForPetlju(trenutneOperacije, rezultat);
            }
        } else {
            Operacija op = razdvojiOperaciju(linija);
            rezultat.push_back(op);
        }
    }

    return rezultat;
}






int main() {
    //koristila sam ovaj primjer radi lakseg pracenja i snalazenja
    string operacije = R"(novac = zaradi(posao)
        mlijeko,jaja,meso,limun = KupiNamirnice(novac)
        zacini,povrce = KupiNamirnice2(novac)
        kuhano_meso = SkuhajMeso(meso, zacini)
        sok = NapraviSok(limun, secer)
        FOR BEGIN
            nesto = nesto(kuhano_meso)
            voda = cesma(otvori)
            nesto2 = nesto2(nesto)
        END
        oprano_povrce = OperiPovrce(povrce, voda)

        escajg = PostaviPribor(sto)
        servirano_jelo = Serviraj(kuhano_meso, oprano_povrce, sok)
        FOR BEGIN
            auto = vozi(gorivo)
        END
    )";


    //testiranje ulaza bez zavisnosti
     /*   string operacije = R"(
        a = Operacija1(x)
        b = Operacija2(y)
        c = Operacija3(z)
        d = Operacija4(w)
        e = Operacija5(v)
    )";*/


   /* string operacije = R"(a,b = Operacija(c, d, e)
                            k,f = Operacija(b, h)
                            b = Operacija(d, r)
                            FOR BEGIN
                                    z = Operacija(b)
                                    u = Operacija(a)
                            END
                            y,g = Operacija(u, v)
                            z = Operacija(f))";*/

    vector<Operacija> operacijeVektor = razdvojiOperacije(operacije);

   /* for(auto op : operacijeVektor) {
        cout<<op;
    }*/


    Graf graf;
    graf.kreirajGraf(operacijeVektor);


   vector<int> topoloskiRed = topoloskoSortiranje(graf);
   cout << "Pomijesani redoslijed operacija: ";
    if (!topoloskiRed.empty()) {

        for (int i : topoloskiRed)
            cout << i + 1<<" ";
    }


   cout<<endl;

    if (!topoloskiRed.empty()) {

        for (int i : topoloskiRed) {
            cout << "Operacija " << i + 1 << ": " << graf.cvorovi[i]->op <<endl;
        }
    }


    return 0;
}
