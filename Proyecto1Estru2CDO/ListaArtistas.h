// ListaArtistas.h
#pragma once

#pragma managed(push, off)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef interface
#undef IServiceProvider
#pragma managed(pop)

#include "Nodo.h"
#include "Artistas.h"

using namespace System;
using namespace System::Collections::Generic;

ref class ListaArtistas
{
private:
    Nodo<Artistas^>^ head;

    // Comparador por nombre artístico (System::String^ directo)
    static int CompareByName(Artistas^ a, Artistas^ b)
    {
        return String::Compare(
            a->Usuario,
            b->Usuario,
            StringComparison::OrdinalIgnoreCase
        );
    }

    // Comparador por ID
    static int CompareById(Artistas^ a, Artistas^ b)
    {
        return a->Id - b->Id;
    }

public:
    ListaArtistas() : head(nullptr) {}

    // Inserta al final
    void Agregar(Artistas^ artista)
    {
        auto nuevo = gcnew Nodo<Artistas^>(artista);
        if (!head) head = nuevo;
        else {
            auto cur = head;
            while (cur->next) cur = cur->next;
            cur->next = nuevo;
        }
    }

    // Búsqueda estilo "binary-search" por nombre artístico
    Nodo<Artistas^>^ Buscar(Nodo<Artistas^>^ tail, String^ clave)
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
                slow->data->Usuario,
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
    Nodo<Artistas^>^ BuscarPorId(int id)
    {
        for (auto cur = head; cur; cur = cur->next)
            if (cur->data->Id == id)
                return cur;
        return nullptr;
    }

    void SortByName()
    {
        auto temp = gcnew List<Artistas^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Artistas^>(CompareByName));

        head = nullptr;
        for each (auto a in temp)
            Agregar(a);
    }

    // Ordena la lista por ID
    void SortById()
    {
        auto temp = gcnew List<Artistas^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Artistas^>(CompareById));

        head = nullptr;
        for each (auto a in temp)
            Agregar(a);
    }
    // Elimina el primer artista cuyo Id coincida. Devuelve true si lo encontró.
    bool EliminarPorId(int id)
    {
        Nodo<Artistas^>^ prev = nullptr;
        Nodo<Artistas^>^ cur = head;

        while (cur != nullptr)
        {
            Artistas^ a = cur->data;
            if (a != nullptr && a->Id == id)
            {
                // desconectar el nodo
                if (prev == nullptr) head = cur->next;  // era el primero
                else                 prev->next = cur->next;

                cur->next = nullptr; // opcional: ayuda al GC
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false; // no encontrado
    }

    // Recorre e imprime
    void Recorrer()
    {
        for (auto cur = head; cur; cur = cur->next)
        {
            Console::WriteLine(
                "{0} (ID={1})",
                cur->data->NombreArtistico,
                cur->data->Id
            );
        }
    }
};
