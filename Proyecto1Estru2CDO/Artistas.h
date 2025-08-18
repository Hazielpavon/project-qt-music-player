#pragma once
#undef IServiceProvider
#include "ListaCanciones.h"
using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;
#include "ListaAlbumes.h" 
#include "Album.h"    
// Forward declaration para evitar dependencias cíclicas
ref class Cancion;

public ref class Artistas
{
private:
    String^ usuario_;
    String^ correoElectronico_;
    String^ nombreArtistico_;
    String^ nombreReal_;
    Image^ imagenPerfil_;
    String^ contrasena_;
    DateTime  fechaNacimiento_;
    String^ pais_;
    String^ genero_;
    bool      estado_; // <-- Campo nuevo
    int       id_;
    ListaCanciones^ listaCanciones;
    ListaAlbumes^ albumes_; 
public:
    // Constructor
    Artistas(
        int        id,
        String^ usuario,
        String^ correoElectronico,
        String^ nombreArtistico,
        String^ nombreReal,
        Image^ imagenPerfil,
        String^ contrasena,
        DateTime   fechaNacimiento,
        String^ pais,
        String^ genero,
        bool       estado // <-- Nuevo parámetro en el constructor
    );

    // --------- Propiedades ----------
    property ListaAlbumes^ Albumes { ListaAlbumes^ get(); }
    property ListaCanciones^ Canciones { ListaCanciones^ get(); }


    property String^ Usuario {
        String^ get();
        void    set(String^);
    }
    property int Id {
        int get();
        void set(int value);
    }
    property String^ CorreoElectronico {
        String^ get();
        void    set(String^);
    }
    property String^ NombreArtistico {
        String^ get();
        void    set(String^);
    }
    property String^ NombreReal {
        String^ get();
        void    set(String^);
    }
    property Image^ ImagenPerfil {
        Image^ get();
        void   set(Image^);
    }
    property String^ Contrasena {
        String^ get();
        void    set(String^);
    }
    property DateTime FechaNacimiento {
        DateTime get();
        void     set(DateTime);
    }
    property String^ Pais {
        String^ get();
        void    set(String^);
    }
    property String^ Genero {
        String^ get();
        void    set(String^);
    }
    property bool Estado { // <-- Nueva propiedad
        bool get();
        void set(bool);
    }
    // En public:
    bool     EliminarCancionPorId(int idUnico);
    Cancion^ BuscarCancionPorId(int idUnico);
    void     LimpiarCanciones();
    int      TotalCanciones();

    void AgregarCancion(Cancion^ c);
    void AgregarAlbum(Album^ a);
    bool EliminarAlbumPorId(int id);
    Album^ BuscarAlbumPorId(int id);
    int   TotalAlbumes();
};
