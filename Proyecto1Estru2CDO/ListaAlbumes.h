#pragma once
#include "Nodo.h"
using namespace System;

// Forward
ref class Album;

ref class ListaAlbumes {
private:
	Nodo<Album^>^ head;

	static int CompareByTitulo(Album^ a, Album^ b);
	static int CompareById(Album^ a, Album^ b);

public:
	ListaAlbumes() : head(nullptr) {};
	void Agregar(Album^ al);
	Nodo<Album^>^ BuscarPorId(int id);
	Nodo<Album^>^ BuscarPorTitulo(String^ t);
	void SortByTitulo();
	void SortById();
	bool EliminarPorId(int id);
	void Limpiar();
	int  Contar();
public: System::Collections::Generic::List<Album^>^ ToList() {
	auto v = gcnew System::Collections::Generic::List<Album^>();
	for (auto cur = head; cur; cur = cur->next) v->Add(cur->data);
	return v;
}


	  // (opcional) Recorrer() para debug
};
