#pragma once
using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Cancion {
private:
    int idUnico;
    String^ titulo;
    String^ artista;
    String^ genero;
    String^ categoria;
    double duracion;
    String^ descripcion;
    String^ rutaAudio;
    Image^ rutaImagen;
    bool estado;
    String^ fechaCarga;
    bool single; // <-- Nuevo campo

public:
    // Constructor
    Cancion(int idUnico,
        String^ titulo,
        String^ artista,
        String^ genero,
        String^ categoria,
        double duracion,
        String^ descripcion,
        String^ rutaAudio,
        Image^ rutaImagen,
        bool estado,
        String^ fechaCarga,
        bool single // <-- Nuevo parámetro
    );

    // Getters
    int getIdUnico();
    String^ getTitulo();
    String^ getArtista();
    String^ getGenero();
    String^ getCategoria();
    double getDuracion();
    String^ getDescripcion();
    String^ getRutaAudio();
    Image^ getRutaImagen();
    bool getEstado();
    String^ getFechaCarga();
    bool getSingle(); // <-- Getter nuevo

    // Setters
    void setIdUnico(int id);
    void setTitulo(String^ titulo);
    void setArtista(String^ artista);
    void setGenero(String^ genero);
    void setCategoria(String^ categoria);
    void setDuracion(double duracion);
    void setDescripcion(String^ descripcion);
    void setRutaAudio(String^ ruta);
    void setRutaImagen(Image^ ruta);
    void setEstado(bool estado);
    void setFechaCarga(String^ fecha);
    void setSingle(bool single); // <-- Setter nuevo
};
