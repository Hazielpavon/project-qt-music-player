// ListaUsuarios.h
#pragma once

#pragma managed(push, off)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef interface
#undef IServiceProvider
#pragma managed(pop)

#include "Nodo.h"
#include "Usuarios.h"
using namespace System;
using namespace System::Collections::Generic;

ref class ListaUsuarios
{
private:
    Nodo<Usuarios^>^ head;

    // Comparador por nombre de usuario (ya son System::String^)
    static int CompareByName(Usuarios^ a, Usuarios^ b)
    {
        return String::Compare(
            a->getUsername(),
            b->getUsername(),
            StringComparison::OrdinalIgnoreCase
        );
    }

    static int CompareById(Usuarios^ a, Usuarios^ b)
    {
        return a->getId() - b->getId();
    }

public:
    ListaUsuarios() : head(nullptr) {}

    void Agregar(Usuarios^ usuario)
    {
        auto nuevo = gcnew Nodo<Usuarios^>(usuario);
        if (!head) head = nuevo;
        else {
            auto cur = head;
            while (cur->next) cur = cur->next;
            cur->next = nuevo;
        }
    }

    // Búsqueda estilo "binary-search" por nombre
    Nodo<Usuarios^>^ Buscar(Nodo<Usuarios^>^ tail, String^ clave)
    {
        for (auto start = head; start != tail; )
        {
            auto slow = start;
            auto fast = start;
            while (fast != tail && fast->next != tail)
            {
                slow = slow->next;
                fast = fast->next->next;
            }
            int cmp = String::Compare(
                slow->data->getUsername(),
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
    Nodo<Usuarios^>^ BuscarPorId(int id)
    {
        for (auto cur = head; cur; cur = cur->next)
            if (cur->data->getId() == id)
                return cur;
        return nullptr;
    }

    void SortByName()
    {
        auto temp = gcnew List<Usuarios^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Usuarios^>(CompareByName));

        head = nullptr;
        for each (auto u in temp)
            Agregar(u);
    }

    void SortById()
    {
        auto temp = gcnew List<Usuarios^>();
        for (auto cur = head; cur; cur = cur->next)
            temp->Add(cur->data);
        temp->Sort(gcnew Comparison<Usuarios^>(CompareById));

        head = nullptr;
        for each (auto u in temp)
            Agregar(u);
    }

    void Recorrer()
    {
        for (auto cur = head; cur; cur = cur->next)
            Console::WriteLine("{0} (ID={1})",
                cur->data->getUsername(),
                cur->data->getId()
            );
    }
};
