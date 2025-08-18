#include "Cancion.h"

// --------- Constructor ----------
Cancion::Cancion(
    int idUnico,
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
)
{
    this->idUnico = idUnico;
    this->titulo = titulo;
    this->artista = artista;
    this->genero = genero;
    this->categoria = categoria;
    this->duracion = duracion;
    this->descripcion = descripcion;
    this->rutaAudio = rutaAudio;
    this->rutaImagen = rutaImagen;
    this->estado = estado;
    this->fechaCarga = fechaCarga;
    this->single = single; // <-- Inicialización
}

// --------- Getters ----------
int Cancion::getIdUnico() { return idUnico; }
String^ Cancion::getTitulo() { return titulo; }
String^ Cancion::getArtista() { return artista; }
String^ Cancion::getGenero() { return genero; }
String^ Cancion::getCategoria() { return categoria; }
double Cancion::getDuracion() { return duracion; }
String^ Cancion::getDescripcion() { return descripcion; }
String^ Cancion::getRutaAudio() { return rutaAudio; }
Image^ Cancion::getRutaImagen() { return rutaImagen; }
bool Cancion::getEstado() { return estado; }
String^ Cancion::getFechaCarga() { return fechaCarga; }
bool Cancion::getSingle() { return single; } // <-- Nuevo getter

// --------- Setters ----------
void Cancion::setIdUnico(int id) { idUnico = id; }
void Cancion::setTitulo(String^ titulo) { this->titulo = titulo; }
void Cancion::setArtista(String^ artista) { this->artista = artista; }
void Cancion::setGenero(String^ genero) { this->genero = genero; }
void Cancion::setCategoria(String^ categoria) { this->categoria = categoria; }
void Cancion::setDuracion(double duracion) { this->duracion = duracion; }
void Cancion::setDescripcion(String^ descripcion) { this->descripcion = descripcion; }
void Cancion::setRutaAudio(String^ ruta) { rutaAudio = ruta; }
void Cancion::setRutaImagen(Image^ ruta) { rutaImagen = ruta; }
void Cancion::setEstado(bool estado) { this->estado = estado; }
void Cancion::setFechaCarga(String^ fecha) { this->fechaCarga = fecha; }
void Cancion::setSingle(bool single) { this->single = single; } // <-- Nuevo setter

