#include "ListaAlbumes.h"
#include "Album.h"
using namespace System::Collections::Generic;

int ListaAlbumes::CompareByTitulo(Album^ a, Album^ b) {
    return String::Compare(a->Titulo, b->Titulo, StringComparison::OrdinalIgnoreCase);
}
int ListaAlbumes::CompareById(Album^ a, Album^ b) {
    return Comparer<int>::Default->Compare(a->Id, b->Id);
}

void ListaAlbumes::Agregar(Album^ al) {
    if (!al) return;
    auto n = gcnew Nodo<Album^>(al);
    if (!head) { head = n; return; }
    auto cur = head; while (cur->next) cur = cur->next; cur->next = n;
}



Nodo<Album^>^ ListaAlbumes::BuscarPorId(int id) {
    for (auto c = head; c; c = c->next) if (c->data && c->data->Id == id) return c;
    return nullptr;
}
Nodo<Album^>^ ListaAlbumes::BuscarPorTitulo(String^ t) {
    for (auto c = head; c; c = c->next)
        if (c->data && String::Compare(c->data->Titulo, t, StringComparison::OrdinalIgnoreCase) == 0)
            return c;
    return nullptr;
}

void ListaAlbumes::SortByTitulo() {
    auto v = gcnew List<Album^>();
    for (auto c = head; c; c = c->next) v->Add(c->data);
    v->Sort(gcnew Comparison<Album^>(CompareByTitulo));
    head = nullptr;
    Nodo<Album^>^ tail = nullptr;
    for each (auto a in v) {
        auto n = gcnew Nodo<Album^>(a);
        if (!head) head = tail = n; else { tail->next = n; tail = n; }
    }
}
void ListaAlbumes::SortById() {
    auto v = gcnew List<Album^>();
    for (auto c = head; c; c = c->next) v->Add(c->data);
    v->Sort(gcnew Comparison<Album^>(CompareById));
    head = nullptr;
    Nodo<Album^>^ tail = nullptr;
    for each (auto a in v) {
        auto n = gcnew Nodo<Album^>(a);
        if (!head) head = tail = n; else { tail->next = n; tail = n; }
    }
}

bool ListaAlbumes::EliminarPorId(int id) {
    if (!head) return false;
    if (head->data && head->data->Id == id) { head = head->next; return true; }
    auto cur = head;
    while (cur->next) {
        if (cur->next->data && cur->next->data->Id == id) {
            cur->next = cur->next->next; return true;
        }
        cur = cur->next;
    }
    return false;
}
void ListaAlbumes::Limpiar() { head = nullptr; }
int  ListaAlbumes::Contar() { int n = 0; for (auto c = head; c; c = c->next) ++n; return n; }
