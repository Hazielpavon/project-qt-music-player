#include "Artistas.h"
#include "Cancion.h"

// --------- Constructor ----------
Artistas::Artistas(
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
    bool       estado 
)
{
    albumes_ = gcnew ListaAlbumes();
    id_ = id;
    usuario_ = usuario;
    correoElectronico_ = correoElectronico;
    nombreArtistico_ = nombreArtistico;
    nombreReal_ = nombreReal;
    imagenPerfil_ = imagenPerfil;
    contrasena_ = contrasena;
    fechaNacimiento_ = fechaNacimiento;
    pais_ = pais;
    genero_ = genero;
    estado_ = estado; // <-- inicialización

    // Inicializa la lista de canciones
    listaCanciones = gcnew ListaCanciones(); 
}
bool Artistas::EliminarCancionPorId(int idUnico) {
    bool removed = false;

    // 1) Quitar de la lista específica del artista
    if (this->listaCanciones && this->listaCanciones->EliminarPorId(idUnico))
        removed = true;

    // 2) Quitar de todos los álbumes del artista
    if (this->albumes_) {
        // Requiere que ListaAlbumes tenga ToList(); si no lo tienes, añádelo.
        auto albs = this->albumes_->ToList();
        for each (Album ^ a in albs) {
            if (a != nullptr && a->Pistas != nullptr) {
                if (a->Pistas->EliminarPorId(idUnico))
                    removed = true;
            }
        }
    }
    return removed;
}

Cancion^ Artistas::BuscarCancionPorId(int idUnico) {
    if (!this->listaCanciones) return nullptr;
    auto n = this->listaCanciones->BuscarPorId(idUnico);
    return n ? n->data : nullptr;
}

void Artistas::LimpiarCanciones() {
    if (this->listaCanciones) this->listaCanciones->Limpiar();
}

int Artistas::TotalCanciones() {
    return this->listaCanciones ? this->listaCanciones->Contar() : 0;
}
// --------- Getters/Setters existentes ----------
String^ Artistas::Usuario::get() { return usuario_; }
void    Artistas::Usuario::set(String^ v) { usuario_ = v; }
bool Artistas::EliminarAlbumPorId(int id) {
    return albumes_ ? albumes_->EliminarPorId(id) : false;
}
int Artistas::Id::get() { return id_; }
void Artistas::Id::set(int value) { id_ = value; }

String^ Artistas::CorreoElectronico::get() { return correoElectronico_; }
void    Artistas::CorreoElectronico::set(String^ v) { correoElectronico_ = v; }

String^ Artistas::NombreArtistico::get() { return nombreArtistico_; }
void    Artistas::NombreArtistico::set(String^ v) { nombreArtistico_ = v; }

String^ Artistas::NombreReal::get() { return nombreReal_; }
void    Artistas::NombreReal::set(String^ v) { nombreReal_ = v; }

Image^ Artistas::ImagenPerfil::get() { return imagenPerfil_; }
void   Artistas::ImagenPerfil::set(Image^ v) { imagenPerfil_ = v; }

String^ Artistas::Contrasena::get() { return contrasena_; }
void    Artistas::Contrasena::set(String^ v) { contrasena_ = v; }

DateTime Artistas::FechaNacimiento::get() { return fechaNacimiento_; }
void     Artistas::FechaNacimiento::set(DateTime v) { fechaNacimiento_ = v; }

String^ Artistas::Pais::get() { return pais_; }
void    Artistas::Pais::set(String^ v) { pais_ = v; }

String^ Artistas::Genero::get() { return genero_; }
void    Artistas::Genero::set(String^ v) { genero_ = v; }


// --------- Nuevo Getter/Setter de Estado ----------
bool Artistas::Estado::get() { return estado_; }
void Artistas::Estado::set(bool v) { estado_ = v; }


void Artistas::AgregarCancion(Cancion^ c) {
    if (c == nullptr) return;
    if (listaCanciones == nullptr) listaCanciones = gcnew ListaCanciones();


    listaCanciones->Agregar(c);
    listaCanciones->SortByName();
}
#include "Album.h"
#include "ListaAlbumes.h"

ListaAlbumes^ Artistas::Albumes::get() { return albumes_; }



void Artistas::AgregarAlbum(Album^ a) {
    if (!a) return;
    if (!albumes_) albumes_ = gcnew ListaAlbumes();
    if (albumes_->BuscarPorId(a->Id) != nullptr) return; // evita duplicado
    albumes_->Agregar(a);
    albumes_->SortByTitulo();
}

Album^ Artistas::BuscarAlbumPorId(int id) {
    auto n = albumes_ ? albumes_->BuscarPorId(id) : nullptr;
    return n ? n->data : nullptr;
}
int Artistas::TotalAlbumes() { return albumes_ ? albumes_->Contar() : 0; }

ListaCanciones^ Artistas::Canciones::get()
{
    return listaCanciones;
}
