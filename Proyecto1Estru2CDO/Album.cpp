#include "Album.h"
#include "Cancion.h"
#include "ListaCanciones.h"

Album::Album(int id, String^ titulo, Image^ portada, DateTime fecha,
    String^ genero, String^ categoria, String^ descripcion,
    bool publicado)
    : id_(id), titulo_(titulo), portada_(portada), descripcion_(descripcion),
    fecha_(fecha), genero_(genero), categoria_(categoria), publicado_(publicado),
    pistas_(gcnew ListaCanciones())
{}

int      Album::Id::get() { return id_; }
void     Album::Id::set(int v) { id_ = v; }
String^ Album::Titulo::get() { return titulo_; }
void     Album::Titulo::set(String^ v) { titulo_ = v; }
Image^ Album::Portada::get() { return portada_; }
void     Album::Portada::set(Image^ v) { portada_ = v; }
String^ Album::Descripcion::get() { return descripcion_; }
void     Album::Descripcion::set(String^ v) { descripcion_ = v; }
DateTime Album::Fecha::get() { return fecha_; }
void     Album::Fecha::set(DateTime v) { fecha_ = v; }
String^ Album::Genero::get() { return genero_; }
void     Album::Genero::set(String^ v) { genero_ = v; }
String^ Album::Categoria::get() { return categoria_; }
void     Album::Categoria::set(String^ v) { categoria_ = v; }
bool     Album::Publicado::get() { return publicado_; }
void     Album::Publicado::set(bool v) { publicado_ = v; }
ListaCanciones^ Album::Pistas::get() { return pistas_; }

void Album::AgregarCancion(Cancion^ c) {
    if (!c || !pistas_) return;
    if (pistas_->BuscarPorId(c->getIdUnico()) != nullptr) return; // evita duplicados
    pistas_->Agregar(c);
    // Si quieres mantener alfabético dentro del álbum:
    // pistas_->SortByName();
}

bool Album::EliminarCancionPorId(int idUnico) {
    if (!pistas_) return false;
    // Añade en ListaCanciones el método EliminarPorId como te mostré antes
    return pistas_->EliminarPorId(idUnico);
}

Cancion^ Album::BuscarCancionPorId(int idUnico) {
    if (!pistas_) return nullptr;
    auto n = pistas_->BuscarPorId(idUnico);
    return n ? n->data : nullptr;
}

