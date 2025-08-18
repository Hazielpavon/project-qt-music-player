#pragma once
#pragma managed(push, off)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef interface
#undef IServiceProvider
#pragma managed(pop)

#include "Nodo.h"
#include  "Cancion.h"
using namespace System;
using namespace System::Collections::Generic;

ref class ListaCanciones
{
private: 
    Nodo<Cancion^>^ head;
    static int CompareByName(Cancion^ a, Cancion^ b)
    {
        return String::Compare(
            a->getTitulo(),
            b->getTitulo()
            ,
            StringComparison::OrdinalIgnoreCase
        );
    }

    // Comparador por ID
    static int CompareById(Cancion^ a, Cancion^ b)
    {
        return a->getIdUnico() - b->getIdUnico();
    }
public:
    ListaCanciones() : head(nullptr) {}

    // Inserta al final
    void Agregar(Cancion^ artista)
    {
        auto nuevo = gcnew Nodo<Cancion^>(artista);
        if (!head) head = nuevo;
        else {
            auto cur = head;
            while (cur->next) cur = cur->next;
            cur->next = nuevo;
        }
    }

    // Búsqueda estilo "binary-search" por nombre artístico
    Nodo<Cancion^>^ Buscar(Nodo<Cancion^>^ tail, String^ clave)
    {
        for (auto start = head; start != tail; )
        {
            auto slow = start;
            auto fast = start;
            while (fast != tail && fast->next != tail) {
                slow = slow->next;
                fast = fast->next->next;
            }

            int cmp = String::Compare(
                slow->data->getTitulo(),
                clave,
                StringComparison::OrdinalIgnoreCase
            );
            if (cmp == 0)       return slow;
            else if (cmp < 0)   start = slow->next;
            else                tail = slow;
        }
        return nullptr;
    }

    // Búsqueda lineal por ID
    Nodo<Cancion^>^ BuscarPorId(int id)
    {
        for (auto cur = head; cur; cur = cur->next)
            if (cur->data->getIdUnico() == id)
                return cur;
        return nullptr;
    }

    void SortByName()
    {
        auto temp = gcnew List<Cancion^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Cancion^>(CompareByName));

        head = nullptr;
        for each (auto a in temp)
            Agregar(a);
    }

    // Ordena la lista por ID
    void SortById()
    {
        auto temp = gcnew List<Cancion^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Cancion^>(CompareById));

        head = nullptr;
        for each (auto a in temp)
            Agregar(a);
    }

    // Recorre e imprime
    void Recorrer()
    {
        for (auto cur = head; cur; cur = cur->next)
        {
            Console::WriteLine(
                "{0} (ID={1})",
                cur->data->getTitulo(),
                cur->data->getArtista()
            );
        }

    }
    // Elimina la primera canción cuyo Id coincida.
// Devuelve true si eliminó, false si no la encontró.
    bool EliminarPorId(int id) {
        if (head == nullptr) return false;

        if (head->data != nullptr && head->data->getIdUnico() == id) {
            head = head->next;
            return true;
        }

        auto prev = head;
        auto cur = head->next;
        while (cur != nullptr) {
            if (cur->data != nullptr && cur->data->getIdUnico() == id) {
                prev->next = cur->next;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    void Limpiar() {
        head = nullptr; // los nodos quedan para el GC
    }

    int Contar() {
        int n = 0;
        for (auto c = head; c != nullptr; c = c->next) ++n;
        return n;
    }

    public: System::Collections::Generic::List<Cancion^>^ ToList() {
        auto v = gcnew System::Collections::Generic::List<Cancion^>();
        for (auto cur = head; cur; cur = cur->next) v->Add(cur->data);
        return v;
    }

};



    
