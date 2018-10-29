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

        Node(N value, bool entrada, bool salida):data(value),isEntrada(entrada),isSalida(salida){}

        N getNdata(){
          return data;
        }

        bool getEntrada(){
          return isEntrada;
        }

        bool getSalida(){
          return isSalida;
        }

    private:
        N data; //Tipo de dato N del trait
        bool isSalida;
        bool isEntrada;
};

#endif
