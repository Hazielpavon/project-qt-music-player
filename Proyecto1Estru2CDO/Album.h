#pragma once
using namespace System;
using namespace System::Drawing;

// Forward declarations para evitar ciclos en .h
ref class Cancion;
ref class ListaCanciones;

public ref class Album
{
private:
    int        id_;
    String^ titulo_;
    Image^ portada_;
    String^ descripcion_;
    DateTime   fecha_;
    String^ genero_;
    String^ categoria_;
    bool       publicado_;             // publicado o no
    ListaCanciones^ pistas_;           // canciones del álbum (tu ListaCanciones)

public:
    Album(int id, String^ titulo, Image^ portada, DateTime fecha,
        String^ genero, String^ categoria, String^ descripcion,
        bool publicado);

    // Props
    property int      Id { int get();    void set(int); }
    property String^ Titulo { String^ get(); void set(String^); }
    property Image^ Portada { Image^ get();  void set(Image^); }
    property String^ Descripcion { String^ get(); void set(String^); }
    property DateTime Fecha { DateTime get(); void set(DateTime); }
    property String^ Genero { String^ get(); void set(String^); }
    property String^ Categoria { String^ get(); void set(String^); }
    property bool     Publicado { bool get();    void set(bool); }
    property ListaCanciones^ Pistas { ListaCanciones^ get(); }   // solo lectura

    // Operaciones sobre las canciones del álbum
    void     AgregarCancion(Cancion^ c);           // agrega al final y (opcional) ordena
    bool     EliminarCancionPorId(int idUnico);
    Cancion^ BuscarCancionPorId(int idUnico);
};
