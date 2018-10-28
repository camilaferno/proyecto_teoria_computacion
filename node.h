#ifndef NODE_H
#define NODE_H

template <typename G>
class Node {
    public:
        typedef typename G::N N;
        typedef typename G::E E;
        typedef typename G::edge edge;
        typedef typename G::EdgeSeq EdgeSeq;

        EdgeSeq edges; //lista de adyacencia, list de edges

        Node(N value,double x, double y):data(value),x(x),y(y){}

        N getNdata(){
          return data;
        }

        double getXdata(){
          return x;
        }

        double getYdata(){
          return y;
        }

    private:
        N data; //Tipo de dato N del trait
        double x; //solo para graficar en librerias como blue
        double y; //solo para graficar en librerias como blue
};

#endif
