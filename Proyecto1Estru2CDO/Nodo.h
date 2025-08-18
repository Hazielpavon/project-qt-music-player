// Nodo.h
#pragma once
generic<typename T> where T : ref class
public ref class Nodo
{
public:
    T data;            // el dato genérico
    Nodo<T>^ next;      // enlace al siguiente nodo del mismo tipo

    Nodo(T value) {
        data = value;
        next = nullptr;
    }
};
