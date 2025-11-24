#include <iostream>
#include <string>
using namespace std;

class Piece{
    protected:
        int nb_windows;
        string couleur;

    public:
         Piece(int w, string c) : nb_windows(w), couleur(c) {}

        virtual ~Piece() {}

     private:
         //getters
         int getW() const{
             return nb_windows;
         }

         string getC() const{
             return couleur;
        }

        //setters 
        void setW(int w){
            nb_windows = w;
        }

        void setC(string c){
            couleur = c;
        }
};

class SalleDeBain : public Piece{
    private:
        bool has_shower;

    public:
        SalleDeBain(int w, string c, bool h) : Piece(w, c), has_shower(h) {}

    private:
        // getters
         bool getH(){
             return has_shower;
            }

         //setters
         void setH(bool h){
            has_shower = h;
        }
    
};

class Chambre : public Piece{
    private:
        int nb_beds;

    public:
        Chambre(int w, string c, int n) : Piece(w, c), nb_beds(n) {}

//getters
    private:
        int getnb_beds(){
            return nb_beds;
        }

//setters
        void setnb_beds(int n){
            nb_beds = n; 
        }
};

class House {
    private:
        int nb_pieces;
        Piece** pieces;

    public:
        House(int np) : nb_pieces(np) {
        pieces = new Piece*[np];
         for(int i=0 ; i<nb_pieces ; i++){
            pieces[i] = nullptr;
         }   
    }

    void addPiece(int index, Piece* p){
            if(index>=0 && index<nb_pieces){
                pieces[index]=p;
            }
    }

    //méthode pour compter le nombre de salles de bain
    int getnb_shower() const{
        int count = 0;
        for(int i=0; i<nb_pieces; i++){
            if(dynamic_cast<SalleDeBain*>(pieces[i]) != nullptr){
            count++;
            }
        }
        return count;
    }


    int getnb_beds() const{
        int count = 0;
        for(int i=0; i<nb_pieces; i++){
            if(dynamic_cast<Chambre*>(pieces[i]) != nullptr){
            count++;
            }
        }
        return count;
    }

    ~House() {
        for (int i = 0; i < nb_pieces; i++) {
            delete pieces[i];
        }
        delete[] pieces;
    }
};

int main(){
    House m(5);
    
    m.addPiece(0, new SalleDeBain(0, "vert", true));
    m.addPiece(1, new Chambre(2, "Blue", 1));
    m.addPiece(2, new Chambre(2, "Bleu", 2));
    m.addPiece(3, new SalleDeBain(1, "Blanc", true));
    m.addPiece(4, new Chambre(1, "Vert", 1));

    cout << " Le nombre de salle de bains est " << m.getnb_shower() << endl;
    cout << " Le nombre de chambres est " << m.getnb_beds() << endl;

return 0;
}