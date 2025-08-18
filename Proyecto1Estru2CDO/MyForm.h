#pragma once
#include <iostream>
#include "Artistas.h"
#include "Nodo.h"
#include "ListaArtistas.h"
#include "Usuarios.h"
#include "ListaUsuarios.h"
#include "Cancion.h"
#include <string>
#include "ListaCanciones.h"
#include "Album.h"
#include "ListaAlbumes.h"
namespace Proyecto1Estru2CDO {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Text::RegularExpressions;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Collections::Generic;
	using namespace WMPLib;
	using namespace AxWMPLib;
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;
	using namespace System::Drawing::Drawing2D;
	public ref class MyForm : public System::Windows::Forms::Form
	{
		// Clases Auiliares 
		//=====================================================================================//
		// === Auxiliar para la UI de carga ===
		ref class DraftSong {
		public:
			System::String^ Path;
			System::String^ Title;
			System::String^ Genero;
			System::String^ Categoria;
			System::String^ Descripcion;
			System::Drawing::Image^ Cover;

			DraftSong(System::String^ p, System::String^ t)
				: Path(p), Title(t), Genero(nullptr), Categoria(nullptr),
				Descripcion(nullptr), Cover(nullptr) {
			}

			bool IsComplete() {
				return !System::String::IsNullOrWhiteSpace(Title) &&
					!System::String::IsNullOrWhiteSpace(Path) &&
					Cover != nullptr &&
					!System::String::IsNullOrWhiteSpace(Genero) &&
					!System::String::IsNullOrWhiteSpace(Categoria);
			}

			virtual System::String^ ToString() override { return Title; }
		};
	private:
		System::Collections::Generic::List<DraftSong^>^ drafts;
		System::Drawing::Image^ draftPlaceholder;
	private: ref class RoundMeta {
	public:
		int Radius;
		int BorderThickness;
		System::Drawing::Color BorderColor;
		RoundMeta(int r, int b, System::Drawing::Color c) : Radius(r), BorderThickness(b), BorderColor(c) {}
	};
		   ref class AlbumRef {
		   public:
			   int id;
			   System::String^ text;
			   AlbumRef(int _id, System::String^ _text) : id(_id), text(_text) {}
			   virtual System::String^ ToString() override { return text; }
		   };
		   ref class SingleRef {
		   public:
			   int songId;
			   System::String^ text;
			   SingleRef(int _songId, System::String^ _text) : songId(_songId), text(_text) {}
			   virtual System::String^ ToString() override { return text; }
		   };
		   ref class SongRef {
		   public:
			   int id;
			   System::String^ text;
			   SongRef(int _id, System::String^ _text) : id(_id), text(_text) {}
			   virtual System::String^ ToString() override { return text; }
		   };
		   ref class SinglesBucketRef {
		   public:
			   System::String^ text;
			   SinglesBucketRef(System::String^ t) : text(t) {}
			   virtual System::String^ ToString() override { return text; }
		   };
		   // FIn CLases Auiliares 
		   //=====================================================================================// 

	private:
		// Declaracion de Variables 
		//=====================================================================================// 
	private: System::Drawing::Image^ x;
	private: System::Drawing::Image^ x2;
	private: Album^ albumSel = nullptr;
	private: Cancion^ cancionSel = nullptr;
	private: bool single = false;
	private: Dictionary<String^, String^>^ songPaths;
	private: Album^ albumEnEdicion = nullptr;
	private: System::Random^ rnd;
	private: Artistas^ artistaActual;
	private: System::Collections::Generic::Dictionary<System::String^, Cancion^>^ pistasListas;
	private:
		::ListaArtistas^ ListaArtistas;
		::ListaUsuarios^ ListaUsuarios;
		::ListaCanciones^ ListaCanciones;
		::ListaCanciones^ NoPublicadas;
	private: System::Windows::Forms::ListBox^ currentLB = nullptr;
	private: int currentIndex = -1;
	private: System::Collections::Generic::Dictionary<String^, System::Drawing::Image^>^ trackCovers;
	private: System::Windows::Forms::Timer^ playTimer;
	private: double mediaDur = 0.0;
	private: System::Windows::Forms::ListBox^ lbReproActual = nullptr;
	private: int idxReproActual = -1;
	private: System::Windows::Forms::Panel^ prFill = nullptr;


		   // Fin Declaracion de Variables 
		   //=====================================================================================// 

		   // Declarando todos los metodos necesarios 
		   //=====================================================================================// 
	private: ::ListaCanciones^ FuentePara(System::Windows::Forms::ListBox^ lb) {
		if (lb == this->listBox6) return this->NoPublicadas;
		return this->ListaCanciones;
	}

	private: Cancion^ CancionFromItem(System::Windows::Forms::ListBox^ lb, System::Object^ item) {
		if (item == nullptr) return nullptr;
		if (auto sref = dynamic_cast<SongRef^>(item)) {
			return GetCancionById(sref->id);
		}
		if (auto title = dynamic_cast<System::String^>(item)) {
			auto fuente = FuentePara(lb);
			if (!fuente) return nullptr;
			auto n = fuente->Buscar(nullptr, title);
			return (n) ? n->data : nullptr;
		}
		return nullptr;
	}
	private: bool StartPlaybackFrom(System::Windows::Forms::ListBox^ lb, int index) {
		if (axWindowsMediaPlayer2 == nullptr) return false;
		if (lb == nullptr || lb->Items->Count == 0) return false;
		if (index < 0 || index >= lb->Items->Count) return false;
		int tries = lb->Items->Count;
		int i = index;
		while (tries-- > 0) {
			auto c = CancionFromItem(lb, lb->Items[i]);
			if (c != nullptr) {
				auto ruta = CleanPath(c->getRutaAudio());
				if (System::String::IsNullOrWhiteSpace(ruta) || !System::IO::File::Exists(ruta)) {
					i = (i + 1) % lb->Items->Count;
					continue;
				}
				currentLB = lb;
				currentIndex = i;
				lb->SelectedIndex = i;

				try {
					axWindowsMediaPlayer2->URL = ruta;
					axWindowsMediaPlayer2->Ctlcontrols->play();
				}
				catch (System::Exception^) {
					MessageBox::Show("No se pudo reproducir la pista.", "Error",
						MessageBoxButtons::OK, MessageBoxIcon::Error);
					return false;
				}
				return true;
			}
			i = (i + 1) % lb->Items->Count;
		}

		MessageBox::Show("No hay pistas reproducibles en la lista.", "Aviso");
		return false;
	}
	private: bool PlayFromList(System::Windows::Forms::ListBox^ lb) {
		if (!lb) return false;
		int idx = (lb->SelectedIndex >= 0) ? lb->SelectedIndex : 0;
		return StartPlaybackFrom(lb, idx);
	}
	private: void NextSong() {
		if (currentLB == nullptr || currentLB->Items->Count == 0) return;
		int next = (currentIndex + 1) % currentLB->Items->Count;
		StartPlaybackFrom(currentLB, next);
	}
	private: void PrevSong() {
		if (currentLB == nullptr || currentLB->Items->Count == 0) return;
		int prev = (currentIndex - 1 + currentLB->Items->Count) % currentLB->Items->Count;
		StartPlaybackFrom(currentLB, prev);
	}


	private: void CargarDatosArtistaEnCampos() {
		if (!artistaActual) return;
		if (textBox26) textBox26->Text = artistaActual->Usuario;
		if (textBox25) textBox25->Text = artistaActual->CorreoElectronico;
		if (textBox24) textBox24->Text = artistaActual->NombreArtistico;
		if (textBox23) textBox23->Text = artistaActual->NombreReal;
		if (comboBox2) SelectComboByText(comboBox2, artistaActual->Genero);
		if (pictureBox63) pictureBox63->Image = artistaActual->ImagenPerfil;
		if (textBox22) textBox22->Clear();
		if (textBox21) textBox21->Clear();
	}

	private: void LimpiarCargaActual() {
		if (axWindowsMediaPlayer2) axWindowsMediaPlayer2->Ctlcontrols->stop();
		if (NoPublicadas)     NoPublicadas = gcnew ::ListaCanciones();
		if (pistasListas)     pistasListas->Clear();
		if (songPaths)        songPaths->Clear();
		if (trackCovers)      trackCovers->Clear();
		if (listBox6) {
			listBox6->Items->Clear();
			listBox6->ClearSelected();
		}
		if (label92)          label92->Text = "0 Archivos";
		if (textBox14)        textBox14->Clear();
		if (textBox15)        textBox15->Clear();
		if (comboBox1)        comboBox1->SelectedIndex = -1;
		if (comboBox3)        comboBox3->SelectedIndex = -1;
		if (pictureBox58)     pictureBox58->Image = x2;
		if (textBox7) {
			textBox7->Clear();
			textBox7->Enabled = !(checkBox3 && checkBox3->Checked);
		}
		if (btnAgregarAudios) btnAgregarAudios->Enabled = true;
	}

	private: Cancion^ FindByTitle(::ListaCanciones^ lc, System::String^ title) {
		if (!lc || System::String::IsNullOrWhiteSpace(title)) return nullptr;
		for each (Cancion ^ c in lc->ToList()) {
			if (c && System::String::Compare(c->getTitulo(), title,
				System::StringComparison::OrdinalIgnoreCase) == 0)
				return c;
		}
		return nullptr;
	}

	private: void UpdateAddButtonState() {
		if (!btnAgregarAudios) return;
		bool singleMode = (checkBox3 && checkBox3->Checked);
		int count = (listBox6 ? listBox6->Items->Count : 0);
		bool hasSel = (listBox6 && listBox6->SelectedIndex >= 0);

		if (singleMode) {
			btnAgregarAudios->Enabled = (count == 0);
		}
		else {
			btnAgregarAudios->Enabled = !hasSel;
		}
	}

	private: void UpdateAlbumNameState() {
		if (!textBox7) return;
		bool singleMode = (checkBox3 && checkBox3->Checked);
		bool hasSel = (listBox6 && listBox6->SelectedIndex >= 0);
		textBox7->Enabled = !singleMode && !hasSel;
	}

	private:int generarMusica() {
		ListaCanciones->SortById();
		int n3;
		do {
			n3 = this->rnd->Next(1000000, 9999999);
		} while (ListaCanciones->BuscarPorId(n3) != nullptr);

		return n3;
	}

	private: int generarAlbumId() {
		if (this->artistaActual == nullptr)
			return this->rnd->Next(1000000, 9999999);

		if (this->artistaActual->Albumes != nullptr)
			this->artistaActual->Albumes->SortById();

		int id;
		do {
			id = this->rnd->Next(1000000, 9999999);
		} while (this->artistaActual->Albumes != nullptr &&
			this->artistaActual->Albumes->BuscarPorId(id) != nullptr);

		return id;
	}

	private: ::ListaCanciones^ FuenteForLB(System::Windows::Forms::ListBox^ lb) {
		if (lb == this->listBox6)  return this->NoPublicadas;
		if (lb == this->listBox11) return this->artistaActual ? this->artistaActual->Canciones : this->ListaCanciones;
		if (lb == this->listBox12) return this->artistaActual ? this->artistaActual->Canciones : this->ListaCanciones;
		if (lb == this->listBox1)  return this->ListaCanciones;
		return this->ListaCanciones;
	}

	private: Cancion^ CancionFromSelection(System::Windows::Forms::ListBox^ lb, ::ListaCanciones^ fuente) {
		if (!lb || lb->SelectedIndex < 0) return nullptr;
		System::Object^ it = lb->SelectedItem;
		if (auto sref = dynamic_cast<SongRef^>(it)) {
			return GetCancionById(sref->id);
		}
		System::String^ titulo = it ? it->ToString() : nullptr;
		if (!fuente || System::String::IsNullOrWhiteSpace(titulo)) return nullptr;
		auto n = fuente->Buscar(nullptr, titulo);
		return n ? n->data : nullptr;
	}

	private: System::String^ FmtTime(double secs) {
		int s = (int)System::Math::Max(0.0, System::Math::Round(secs));
		int m = s / 60; s = s % 60;
		return System::String::Format("{0}:{1:00}", m, s);
	}

	private: bool PlayFromLB(System::Windows::Forms::ListBox^ lb) {
		auto fuente = FuenteForLB(lb);
		Cancion^ c = CancionFromSelection(lb, fuente);
		if (c == nullptr) { System::Windows::Forms::MessageBox::Show("No se encontro la cancion."); return false; }
		System::String^ ruta = CleanPath(c->getRutaAudio());
		if (System::String::IsNullOrWhiteSpace(ruta) || !System::IO::File::Exists(ruta)) {
			System::Windows::Forms::MessageBox::Show("Ruta inválida o el archivo no existe.");
			return false;
		}

		if (this->label57)  this->label57->Text = c->getTitulo();
		if (this->label93) this->label93->Text = (this->artistaActual ? this->artistaActual->NombreArtistico : "");
		else if (this->label93) this->label93->Text = (this->artistaActual ? this->artistaActual->NombreArtistico : "");
		pictureBox54->Image = c->getRutaImagen(); 

		this->axWindowsMediaPlayer2->URL = ruta;
		this->axWindowsMediaPlayer2->Ctlcontrols->play();
		this->lbReproActual = lb;
		this->idxReproActual = lb->SelectedIndex;
		return true;
	}

	private: void NextTrack() {
		if (!lbReproActual || lbReproActual->Items->Count == 0) return;
		int n = lbReproActual->Items->Count;
		int start = (lbReproActual->SelectedIndex >= 0) ? lbReproActual->SelectedIndex : idxReproActual;
		for (int i = 1; i <= n; ++i) {
			int cand = (start + i) % n;
			lbReproActual->SelectedIndex = cand;
			if (PlayFromLB(lbReproActual)) { idxReproActual = cand; break; }
		}
	}

	private: void PrevTrack() {
		if (!lbReproActual || lbReproActual->Items->Count == 0) return;
		int n = lbReproActual->Items->Count;
		int start = (lbReproActual->SelectedIndex >= 0) ? lbReproActual->SelectedIndex : idxReproActual;
		for (int i = 1; i <= n; ++i) {
			int cand = (start - i + n) % n;
			lbReproActual->SelectedIndex = cand;
			if (PlayFromLB(lbReproActual)) { idxReproActual = cand; break; }
		}
	}

	private: bool PlayFromListBox(System::Windows::Forms::ListBox^ lb, ::ListaCanciones^ fuente) {
		if (axWindowsMediaPlayer2 == nullptr || lb == nullptr || fuente == nullptr) return false;
		if (lb->SelectedIndex < 0) return false;
		fuente->SortByName();
		System::String^ key = lb->SelectedItem->ToString();
		Nodo<Cancion^>^ nodo = fuente->Buscar(nullptr, key);
		if (nodo == nullptr || nodo->data == nullptr) {
			MessageBox::Show("No se encontró la canción.", "Error");
			return false;
		}

		System::String^ ruta = CleanPath(nodo->data->getRutaAudio());
		if (System::String::IsNullOrWhiteSpace(ruta) || !System::IO::File::Exists(ruta)) {
			MessageBox::Show("Ruta inválida o el archivo no existe.", "Error");
			return false;
		}

		axWindowsMediaPlayer2->URL = ruta;
		axWindowsMediaPlayer2->Ctlcontrols->play();
		return true;
	}

	private: bool IsDesign() {
		return (System::ComponentModel::LicenseManager::UsageMode ==
			System::ComponentModel::LicenseUsageMode::Designtime)
			|| (this->Site != nullptr && this->Site->DesignMode);
	}

	private:int generar() {
		ListaUsuarios->SortById();
		ListaArtistas->SortById();
		int n3;
		do {
			n3 = this->rnd->Next(1000000, 9999999);
		} while (ListaArtistas->BuscarPorId(n3) != nullptr
			|| ListaUsuarios->BuscarPorId(n3) != nullptr);

		return n3;
	}

	private: System::String^ NormalizeTitle(System::String^ s) {
		if (System::String::IsNullOrWhiteSpace(s)) return nullptr;
		s = s->Trim();
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder(s->Length);
		bool prevSpace = false;
		for (int i = 0; i < s->Length; ++i) {
			wchar_t ch = s[i];
			bool isSpace = System::Char::IsWhiteSpace(ch);
			if (isSpace) {
				if (!prevSpace) sb->Append(' ');
			}
			else {
				sb->Append(System::Char::ToLowerInvariant(ch));
			}
			prevSpace = isSpace;
		}
		return sb->ToString();
	}

	private: Album^ FindAlbumByTitle(System::String^ titulo) {
		if (!artistaActual || !artistaActual->Albumes) return nullptr;
		auto key = NormalizeTitle(titulo);
		if (key == nullptr) return nullptr;
		for each (Album ^ a in artistaActual->Albumes->ToList()) {
			if (a && System::String::Equals(NormalizeTitle(a->Titulo), key,
				System::StringComparison::Ordinal))
				return a;
		}
		return nullptr;
	}

	private: Album^ EnsureAlbumEnEdicion(System::String^ titulo, System::Drawing::Image^ portada, System::String^ genero, System::String^ categoria)
	{
		auto key = NormalizeTitle(titulo);
		if (key == nullptr) return nullptr;
		if (albumEnEdicion &&
			System::String::Equals(NormalizeTitle(albumEnEdicion->Titulo), key,
				System::StringComparison::Ordinal))
		{
			if (portada) albumEnEdicion->Portada = portada;
			if (!System::String::IsNullOrWhiteSpace(genero))    albumEnEdicion->Genero = genero;
			if (!System::String::IsNullOrWhiteSpace(categoria)) albumEnEdicion->Categoria = categoria;
			return albumEnEdicion;
		}
		if (Album^ existente = FindAlbumByTitle(titulo)) {
			if (portada) existente->Portada = portada;
			if (!System::String::IsNullOrWhiteSpace(genero))    existente->Genero = genero;
			if (!System::String::IsNullOrWhiteSpace(categoria)) existente->Categoria = categoria;
			albumEnEdicion = existente;
			return existente;
		}
		int nuevoId = generarAlbumId();
		Album^ nuevo = gcnew Album(nuevoId, titulo->Trim(), portada, DateTime::Now, genero, categoria, "", false);
		albumEnEdicion = nuevo;
		return nuevo;
	}

	private: bool IsSinglesBucket(System::Object^ item) {
		if (!item) return false;
		if (dynamic_cast<SinglesBucketRef^>(item)) return true;
		System::String^ s = item->ToString();
		if (System::String::IsNullOrWhiteSpace(s)) return false;
		s = s->Trim()->ToLowerInvariant();
		return s == "— singles —" || s == "- singles -" || s->Contains("singles");
	}

	private: static System::String^ CleanPath(System::String^ p) {
		if (p == nullptr) return nullptr;
		p = p->Trim();
		p = p->Trim(gcnew array<wchar_t>{'"', '\''});
		if (p->StartsWith("\\\\?\\")) p = p->Substring(4);
		try { p = System::IO::Path::GetFullPath(p); }
		catch (...) {}
		return p;
	}

	private: void CargarCancionesDeAlbumEn(Album^ alb, System::Windows::Forms::ListBox^ lb) {
		if (!lb) return;
		lb->Items->Clear();
		if (!alb || !alb->Pistas) return;
		for each (Cancion ^ c in SongListFrom(alb->Pistas))
			if (c) lb->Items->Add(gcnew SongRef(c->getIdUnico(), c->getTitulo()));
	}

	private: void CargarCancionSingleEn(int songId, System::Windows::Forms::ListBox^ lb) {
		if (!lb) return;
		lb->Items->Clear();
		if (auto c = GetCancionById(songId))
			lb->Items->Add(gcnew SongRef(c->getIdUnico(), c->getTitulo()));
	}

	private: void EliminarCancionEverywhere(int songId, Album^ albumIfAny) {
		if (albumIfAny && albumIfAny->Pistas) albumIfAny->EliminarCancionPorId(songId);
		if (artistaActual) artistaActual->EliminarCancionPorId(songId);
		if (ListaCanciones) ListaCanciones->EliminarPorId(songId);
	}

	private: void SelectComboByText(System::Windows::Forms::ComboBox^ cb, System::String^ value) {
		if (!cb) return;
		int found = -1;
		for (int i = 0; i < cb->Items->Count; ++i) {
			if (System::String::Compare(cb->Items[i]->ToString(), value, System::StringComparison::OrdinalIgnoreCase) == 0) {
				found = i; break;
			}
		}
		cb->SelectedIndex = found;
	}

	private: void CargarAlbumesDelArtista() {
		if (!listBox8) return;
		listBox8->Items->Clear();
		if (!artistaActual || !artistaActual->Albumes) return;
		for each (Album ^ a in artistaActual->Albumes->ToList())
			if (a) listBox8->Items->Add(gcnew AlbumRef(a->Id, a->Titulo));
		bool haySingles = false;
		if (artistaActual->Canciones) {
			for each (Cancion ^ c in artistaActual->Canciones->ToList()) {
				if (c && c->getSingle()) { haySingles = true; break; }
			}
		}
		if (haySingles)
			listBox8->Items->Insert(0, gcnew SinglesBucketRef("— Singles —"));
	}

	private: void CargarSinglesEnListBox7() {
		listBox7->Items->Clear();
		if (!artistaActual || !artistaActual->Canciones) return;

		for each (Cancion ^ c in artistaActual->Canciones->ToList()) {
			if (c && c->getSingle())
				listBox7->Items->Add(gcnew SongRef(c->getIdUnico(), c->getTitulo()));
		}
	}

	private: void MostrarAlbumEnUI(Album^ alb) {
		albumSel = alb;
		if (alb == nullptr) {
			if (textBox7) textBox7->Text = "";
			return;
		}
		if (textBox7) textBox7->Text = alb->Titulo;
	}

	private: void MostrarCancionEnUI(Cancion^ c) {
		cancionSel = c;
		if (!c) {
			if (textBox17) textBox17->Text = "";
			if (textBox28) textBox28->Text = "";
			if (comboBox5) comboBox5->SelectedIndex = -1;
			if (comboBox4) comboBox4->SelectedIndex = -1;
			if (pictureBox60) pictureBox60->Image = x2;
			return;
		}
		if (textBox17) textBox17->Text = c->getTitulo();
		if (textBox28) textBox28->Text = c->getDescripcion();
		if (comboBox5) SelectComboByText(comboBox5, c->getGenero());
		if (comboBox4) SelectComboByText(comboBox4, c->getCategoria());
		if (pictureBox60) pictureBox60->Image = c->getRutaImagen();
	}

	private: System::Collections::Generic::List<Album^>^ AlbList() {
		return (artistaActual && artistaActual->Albumes) ? artistaActual->Albumes->ToList()
			: gcnew System::Collections::Generic::List<Album^>();
	}
	private: System::Collections::Generic::List<Cancion^>^ SongListFrom(::ListaCanciones^ lc) {
		return lc ? lc->ToList() : gcnew System::Collections::Generic::List<Cancion^>();
	}

	private: bool CancionEstaEnUnAlbumDelArtista(int songId) {
		for each (Album ^ a in AlbList())
			if (a && a->Pistas && a->Pistas->BuscarPorId(songId) != nullptr) return true;
		return false;
	}

	private: Album^ GetAlbum(AlbumRef^ aref) {
		if (!aref || !artistaActual || !artistaActual->Albumes) return nullptr;
		auto n = artistaActual->Albumes->BuscarPorId(aref->id);
		return n ? n->data : nullptr;
	}

	private: Cancion^ GetCancionById(int id) {
		Cancion^ r = nullptr;
		if (this->ListaCanciones) {
			auto n = this->ListaCanciones->BuscarPorId(id);
			if (n) r = n->data;
		}
		if (!r && this->artistaActual && this->artistaActual->Canciones) {
			auto n2 = this->artistaActual->Canciones->BuscarPorId(id);
			if (n2) r = n2->data;
		}
		return r;
	}

	private: void  show(String^ msg, bool% okFlag) {
		MessageBox::Show(msg, "Datos inválidos", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		okFlag = false;
	}

	private: void CargarAlbumsYSinglesEn(System::Windows::Forms::ListBox^ lb, bool conPrefijos) {
		if (!lb) return;
		lb->Items->Clear();
		if (!artistaActual) return;
		for each (Album ^ a in AlbList())
			if (a) lb->Items->Add(gcnew AlbumRef(a->Id, conPrefijos ? "Álbum: " + a->Titulo : a->Titulo));
		for each (Cancion ^ c in SongListFrom(artistaActual->Canciones)) {
			if (c && c->getSingle() && !CancionEstaEnUnAlbumDelArtista(c->getIdUnico()))
				lb->Items->Add(gcnew SingleRef(c->getIdUnico(), conPrefijos ? "Single: " + c->getTitulo() : c->getTitulo()));
		}
	}

	private: void CargarCancionesDeAlbum(Album^ alb) {
		listBox12->Items->Clear();
		if (!alb || !alb->Pistas) return;
		for each (Cancion ^ c in SongListFrom(alb->Pistas))
			if (c) listBox12->Items->Add(gcnew SongRef(c->getIdUnico(), c->getTitulo()));
	}

	private: void CargarCancionSingle(int songId) {
		listBox12->Items->Clear();
		Cancion^ c = GetCancionById(songId);
		if (c) listBox12->Items->Add(gcnew SongRef(c->getIdUnico(), c->getTitulo()));
	}
	private: System::Void LbCommon_MeasureItem(System::Object^, System::Windows::Forms::MeasureItemEventArgs^ e) {
		e->ItemHeight = 68;
	}

	private: System::Drawing::Image^ ThumbForItem(System::Windows::Forms::ListBox^ lb, System::Object^ item) {

		if (lb == this->listBox6) {
			System::String^ key = item ? item->ToString() : nullptr;
			if (!System::String::IsNullOrEmpty(key) && trackCovers && trackCovers->ContainsKey(key))
				return trackCovers[key];
			return nullptr;
		}
		if (auto aRef = dynamic_cast<AlbumRef^>(item)) {
			Album^ a = GetAlbum(aRef);
			return a ? a->Portada : x2;
		}
		if (auto sRef = dynamic_cast<SingleRef^>(item)) {
			Cancion^ c = GetCancionById(sRef->songId);
			return c ? c->getRutaImagen() : x2;
		}
		if (auto sg = dynamic_cast<SongRef^>(item)) {
			Cancion^ c = GetCancionById(sg->id);
			return c ? c->getRutaImagen() : x2;
		}
		return nullptr;
	}

	private: System::Void LbCommon_DrawItem(System::Object^ sender, System::Windows::Forms::DrawItemEventArgs^ e) {
		if (e->Index < 0) return;

		auto lb = safe_cast<System::Windows::Forms::ListBox^>(sender);
		auto item = lb->Items[e->Index];
		System::String^ text = item ? item->ToString() : "";

		bool isSel = ((e->State & System::Windows::Forms::DrawItemState::Selected) == System::Windows::Forms::DrawItemState::Selected);
		System::Drawing::Color bg = isSel
			? System::Drawing::Color::FromArgb(0x44, 0x79, 0x67)
			: System::Drawing::Color::FromArgb(0x30, 0x33, 0x3B);
		e->Graphics->FillRectangle(gcnew System::Drawing::SolidBrush(bg), e->Bounds);

		System::Drawing::Image^ img = ThumbForItem(lb, item);
		System::Drawing::Rectangle rcImg(e->Bounds.Left + 6, e->Bounds.Top + 6, 56, 56);
		if (img) e->Graphics->DrawImage(img, rcImg);
		else     e->Graphics->DrawRectangle(System::Drawing::Pens::DimGray, rcImg);

		System::Drawing::Rectangle rcText(e->Bounds.Left + 72, e->Bounds.Top + 6,
			e->Bounds.Width - 78, 56);
		auto font = gcnew System::Drawing::Font("Segoe UI", 10, System::Drawing::FontStyle::Bold);
		auto br = gcnew System::Drawing::SolidBrush(System::Drawing::Color::White);

		if (dynamic_cast<SinglesBucketRef^>(item))
			font = gcnew System::Drawing::Font("Segoe UI", 10, System::Drawing::FontStyle::Italic);

		e->Graphics->DrawString(text, font, br, rcText);
		e->DrawFocusRectangle();
	}

		   // Fin Declarando Metodos Necesarios 
		   //=====================================================================================// 


		   // Mas Codigo Generado 
		   //=====================================================================================// 
	private: System::Windows::Forms::ProgressBar^ progressBar1;
	private: System::Windows::Forms::Label^ label50;
	private: System::Windows::Forms::Label^ label69;
	private: System::Windows::Forms::HScrollBar^ hScrollBar1;
	private: System::Windows::Forms::Panel^ panel4;
	private: System::Windows::Forms::Panel^ panel9;
	private: System::Windows::Forms::Label^ label93;
	private: System::Windows::Forms::Label^ label75;
	private: System::Windows::Forms::ProgressBar^ progressBar2;
	private: System::Windows::Forms::PictureBox^ pictureBox54;
	private: System::Windows::Forms::PictureBox^ pictureBox52;
	private: System::Windows::Forms::PictureBox^ pictureBox56;
	private: System::Windows::Forms::Panel^ panel10;
	private: System::Windows::Forms::Panel^ panel13;
	private: System::Windows::Forms::Label^ label94;
	private: System::Windows::Forms::Panel^ panel15;
	private: System::Windows::Forms::Panel^ panel16;
	private: System::Windows::Forms::Label^ label96;
	private: System::Windows::Forms::Label^ label95;
	private: System::Windows::Forms::Label^ label97;
	private: System::Windows::Forms::Panel^ panel17;
	private: System::Windows::Forms::Label^ lblConteoArchivos;
	private: System::Windows::Forms::Label^ btnAgregarAudios;
	private: System::Windows::Forms::CheckBox^ checkBox3;
	private: System::Windows::Forms::TextBox^ textBox14;
	private: System::Windows::Forms::Label^ label47;
	private: System::Windows::Forms::Panel^ panel18;
	private: System::Windows::Forms::Panel^ panel19;
	private: System::Windows::Forms::Label^ label58;
	private: System::Windows::Forms::Panel^ panel20;
	private: System::Windows::Forms::Label^ label74;
	private: System::Windows::Forms::Label^ label98;
	private: System::Windows::Forms::Panel^ panel21;
	private: System::Windows::Forms::Label^ label99;
	private: System::Windows::Forms::Label^ label101;
	private: System::Windows::Forms::Label^ label100;
	private: System::Windows::Forms::Panel^ panel22;
	private: System::Windows::Forms::Label^ label103;
	private: System::Windows::Forms::Label^ label102;
	private: System::Windows::Forms::Panel^ panel23;
	private: System::Windows::Forms::Panel^ panel24;
	private: System::Windows::Forms::Label^ label104;
	private: System::Windows::Forms::Label^ label92;
	private: AxWMPLib::AxWindowsMediaPlayer^ axWindowsMediaPlayer1;
	private: System::Windows::Forms::Panel^ panel11;
	private: System::Windows::Forms::Panel^ panel12;
	private: System::Windows::Forms::Label^ label26;
	private: System::Windows::Forms::Label^ label30;
	private: System::Windows::Forms::Label^ label29;
	private: System::Windows::Forms::Label^ label28;
	private: System::Windows::Forms::Label^ label27;
	private: System::Windows::Forms::PictureBox^ pictureBox31;
	private: System::Windows::Forms::PictureBox^ pictureBox30;
	private: System::Windows::Forms::PictureBox^ pictureBox29;
	private: System::Windows::Forms::PictureBox^ pictureBox27;
	private: System::Windows::Forms::PictureBox^ pictureBox8;
	private: System::Windows::Forms::Panel^ PanelCatalogo;
	private: System::Windows::Forms::ListBox^ listBox1;
	private: System::Windows::Forms::Panel^ panel14;
	private: System::Windows::Forms::Label^ label33;
	private: System::Windows::Forms::PictureBox^ pictureBox32;
	private: System::Windows::Forms::PictureBox^ pictureBox38;
	private: System::Windows::Forms::PictureBox^ pictureBox37;
	private: System::Windows::Forms::PictureBox^ pictureBox36;
	private: System::Windows::Forms::PictureBox^ pictureBox35;
	private: System::Windows::Forms::PictureBox^ pictureBox34;
	private: System::Windows::Forms::PictureBox^ pictureBox33;
	private: System::Windows::Forms::TextBox^ textBox3;
	private: System::Windows::Forms::Panel^ PanelVerPlay;
	private: System::Windows::Forms::PictureBox^ pictureBox39;
	private: System::Windows::Forms::TextBox^ textBox4;
	private: System::Windows::Forms::ListBox^ listBox2;
	private: System::Windows::Forms::Panel^ PanelCrearPlay;
	private: System::Windows::Forms::ListBox^ listBox4;
	private: System::Windows::Forms::PictureBox^ pictureBox40;
	private: System::Windows::Forms::TextBox^ textBox5;
	private: System::Windows::Forms::ListBox^ listBox3;
	private: System::Windows::Forms::Label^ label37;
	private: System::Windows::Forms::Label^ label36;
	private: System::Windows::Forms::PictureBox^ pictureBox41;
	private: System::Windows::Forms::Label^ label35;
	private: System::Windows::Forms::TextBox^ textBox6;
	private: System::Windows::Forms::Panel^ PanelFav;
	private: System::Windows::Forms::ListBox^ listBox5;
	private: System::Windows::Forms::PictureBox^ pictureBox43;
	private: System::Windows::Forms::TextBox^ textBox8;
	private: System::Windows::Forms::Panel^ PanelConfig;
	private: System::Windows::Forms::Label^ label38;
	private: System::Windows::Forms::PictureBox^ pictureBox42;
	private: System::Windows::Forms::TextBox^ textBox9;
	private: System::Windows::Forms::Label^ label40;
	private: System::Windows::Forms::TextBox^ textBox10;
	private: System::Windows::Forms::TextBox^ textBox11;
	private: System::Windows::Forms::Button^ button15;
	private: System::Windows::Forms::TextBox^ textBox12;
	private: System::Windows::Forms::TextBox^ textBox13;
	private: System::Windows::Forms::Button^ button17;
	private: System::Windows::Forms::Label^ label42;
	private: System::Windows::Forms::Label^ label43;
	private: System::Windows::Forms::Label^ label44;
	private: System::Windows::Forms::Label^ label45;
	private: System::Windows::Forms::Label^ label46;
	private: System::Windows::Forms::Label^ label39;
	private: System::Windows::Forms::PictureBox^ pictureBox44;
	private: System::Windows::Forms::Panel^ panelArtista;
	private: System::Windows::Forms::Panel^ PanelSubir;
	private: System::Windows::Forms::Panel^ panel6;
	private: System::Windows::Forms::Label^ label15;
	private: System::Windows::Forms::PictureBox^ pictureBox4;
	private: System::Windows::Forms::Label^ label51;
	private: System::Windows::Forms::PictureBox^ pictureBox5;
	private: System::Windows::Forms::Label^ label52;
	private: System::Windows::Forms::PictureBox^ pictureBox45;
	private: System::Windows::Forms::PictureBox^ pictureBox46;
	private: System::Windows::Forms::PictureBox^ pictureBox47;
	private: System::Windows::Forms::PictureBox^ pictureBox48;
	private: System::Windows::Forms::PictureBox^ pictureBox49;
	private: System::Windows::Forms::Label^ label53;
	private: System::Windows::Forms::Label^ label54;
	private: System::Windows::Forms::Label^ label55;
	private: System::Windows::Forms::Label^ label56;
	private: System::Windows::Forms::Panel^ panel7;
	private: System::Windows::Forms::Panel^ panel8;
	private: System::Windows::Forms::PictureBox^ pictureBox51;
	private: System::Windows::Forms::PictureBox^ pictureBox53;
	private: System::Windows::Forms::Label^ label57;
	private: System::Windows::Forms::PictureBox^ pictureBox57;
	private: System::Windows::Forms::Label^ label59;
	private: System::Windows::Forms::Label^ label41;
	private: System::Windows::Forms::PictureBox^ pictureBox58;
	private: System::Windows::Forms::ListBox^ listBox6;
	private: System::Windows::Forms::Label^ label16;
	private: System::Windows::Forms::TextBox^ textBox7;
	private: System::Windows::Forms::Label^ label49;
	private: System::Windows::Forms::Label^ label48;
	private: System::Windows::Forms::Panel^ PanelEditar;
	private: System::Windows::Forms::Label^ label61;
	private: System::Windows::Forms::Label^ label62;
	private: System::Windows::Forms::PictureBox^ pictureBox60;
	private: System::Windows::Forms::TextBox^ textBox17;
	private: System::Windows::Forms::ListBox^ listBox7;
	private: System::Windows::Forms::Label^ label63;
	private: System::Windows::Forms::ListBox^ listBox8;
	private: System::Windows::Forms::Panel^ PanelEliminar;
	private: System::Windows::Forms::ListBox^ listBox9;
	private: System::Windows::Forms::Label^ label64;
	private: System::Windows::Forms::ListBox^ listBox10;
	private: System::Windows::Forms::Panel^ panelDisco;
	private: System::Windows::Forms::ListBox^ listBox11;
	private: System::Windows::Forms::ListBox^ listBox12;
	private: System::Windows::Forms::Panel^ panelStats;
	private: System::Windows::Forms::ListBox^ listBox13;
	private: System::Windows::Forms::Label^ label66;
	private: System::Windows::Forms::Label^ label65;
	private: System::Windows::Forms::Label^ label60;
	private: System::Windows::Forms::ListBox^ listBox17;
	private: System::Windows::Forms::ListBox^ listBox14;
	private: System::Windows::Forms::DataGridView^ dataGridView2;
	private: System::Windows::Forms::Label^ label68;
	private: System::Windows::Forms::DataGridView^ dataGridView1;
	private: System::Windows::Forms::Label^ label67;
	private: System::Windows::Forms::Panel^ PanelConfi;
	private: System::Windows::Forms::Label^ label70;
	private: System::Windows::Forms::Label^ label71;
	private: System::Windows::Forms::ComboBox^ comboBox2;
	private: System::Windows::Forms::TextBox^ textBox21;
	private: System::Windows::Forms::TextBox^ textBox22;
	private: System::Windows::Forms::Button^ button9;
	private: System::Windows::Forms::TextBox^ textBox23;
	private: System::Windows::Forms::TextBox^ textBox24;
	private: System::Windows::Forms::TextBox^ textBox25;
	private: System::Windows::Forms::TextBox^ textBox26;
	private: System::Windows::Forms::Button^ button10;
	private: System::Windows::Forms::Button^ button11;
	private: System::Windows::Forms::Label^ label72;
	private: System::Windows::Forms::Label^ label76;
	private: System::Windows::Forms::Label^ label77;
	private: System::Windows::Forms::Label^ label78;
	private: System::Windows::Forms::Label^ label79;
	private: System::Windows::Forms::Button^ button12;
	private: System::Windows::Forms::PictureBox^ pictureBox64;
	private: System::Windows::Forms::Label^ label73;
	private: System::Windows::Forms::PictureBox^ pictureBox63;
	private: System::Windows::Forms::Panel^ Panelstatsusu;
	private: System::Windows::Forms::DataGridView^ dataGridView4;
	private: System::Windows::Forms::Label^ label80;
	private: System::Windows::Forms::Label^ label81;
	private: System::Windows::Forms::Label^ label82;
	private: System::Windows::Forms::Label^ label83;
	private: System::Windows::Forms::ListBox^ listBox15;
	private: System::Windows::Forms::ListBox^ listBox16;
	private: System::Windows::Forms::ListBox^ listBox18;
	private: System::Windows::Forms::PictureBox^ pictureBox50;
	private: AxWMPLib::AxWindowsMediaPlayer^ axWindowsMediaPlayer2;
	private: System::Windows::Forms::ComboBox^ comboBox1;
	private: System::Windows::Forms::Label^ label84;
	private: System::Windows::Forms::TextBox^ textBox15;
	private: System::Windows::Forms::Label^ label85;
	private: System::Windows::Forms::ComboBox^ comboBox3;
	private: System::Windows::Forms::Label^ label86;
	private: System::Windows::Forms::ComboBox^ comboBox4;
	private: System::Windows::Forms::Label^ label87;
	private: System::Windows::Forms::TextBox^ textBox28;
	private: System::Windows::Forms::Label^ label88;
	private: System::Windows::Forms::ComboBox^ comboBox5;
	private: System::Windows::Forms::Label^ label89;
	private: System::Windows::Forms::Label^ label91;
	private: System::Windows::Forms::Label^ label90;

		   // FIn Codigo Generado 
		   //=====================================================================================// 

		   // Se declara init 
		   //=====================================================================================// 
	public:
		MyForm(void)
		{
			InitializeComponent();

			this->axWindowsMediaPlayer2->PlayStateChange +=
				gcnew AxWMPLib::_WMPOCXEvents_PlayStateChangeEventHandler(
					this, &Proyecto1Estru2CDO::MyForm::AxPlayer_PlayStateChange);

			playTimer = gcnew System::Windows::Forms::Timer();
			playTimer->Interval = 500;
			playTimer->Tick += gcnew System::EventHandler(this, &Proyecto1Estru2CDO::MyForm::PlayTimer_Tick);

			this->progressBar2->Style = ProgressBarStyle::Continuous;
			this->progressBar2->Minimum = 0;
			this->progressBar2->Maximum = 100;
			prFill = gcnew System::Windows::Forms::Panel();
			prFill->BackColor = System::Drawing::Color::PaleVioletRed;
			prFill->Height = progressBar2->Height;
			prFill->Width = 0;
			prFill->Left = progressBar2->Left;
			prFill->Top = progressBar2->Top;
			progressBar2->Parent->Controls->Add(prFill);
			prFill->BringToFront();

			drafts = gcnew System::Collections::Generic::List<DraftSong^>();
			draftPlaceholder = safe_cast<System::Drawing::Image^>(
				pictureBox58->Image ? pictureBox58->Image->Clone() : nullptr);
			listBox6->DrawMode = System::Windows::Forms::DrawMode::OwnerDrawVariable;
			comboBox3->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::draftCategoria_Changed);
			textBox15->TextChanged += gcnew System::EventHandler(this, &MyForm::draftDescripcion_Changed);
			btnAgregarAudios->Click += gcnew System::EventHandler(this, &MyForm::btnAgregarAudios_Click);
			this->listBox7->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox7->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox7->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);

			this->listBox8->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox8->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox8->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);

			this->listBox9->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox9->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox9->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);

			this->listBox10->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox10->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox10->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);

			this->listBox11->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox11->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox11->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);
			this->listBox11->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox11_SelectedIndexChanged);

			this->listBox12->DrawMode = DrawMode::OwnerDrawVariable;
			this->listBox12->MeasureItem += gcnew MeasureItemEventHandler(this, &MyForm::LbCommon_MeasureItem);
			this->listBox12->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::LbCommon_DrawItem);
			this->listBox12->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox12_SelectedIndexChanged);

			if (IsDesign()) return;
			songPaths = gcnew Dictionary<String^, String^>();
			this->rnd = gcnew System::Random();
			this->ListaArtistas = gcnew ::ListaArtistas();
			this->ListaUsuarios = gcnew ::ListaUsuarios();
			this->ListaCanciones = gcnew ::ListaCanciones();
			panel2->Visible = false;
			PanelRegistroArtista->Visible = false;
			PanelRegistroUsuario->Visible = false;
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			panelArtista->Visible = false;
			PanelUsuario->Visible = false;
			PanelFav->Visible = false;
			PanelCrearPlay->Visible = false;
			PanelVerPlay->Visible = false;
			PanelCatalogo->Visible = false;
			Panelstatsusu->Visible = false;
			panelStats->Visible = false;
			PanelConfi->Visible = false;
			panelDisco->Visible = false;
			PanelEliminar->Visible = false;
			PanelEditar->Visible = false;
			PanelSubir->Visible = false;
			x = safe_cast<Image^>(pictureBox3->Image->Clone());
			x2 = safe_cast<Image^>(pictureBox58->Image->Clone());
			pistasListas = gcnew System::Collections::Generic::Dictionary<System::String^, Cancion^>();
			PanelConfig->Visible = false;
			this->listBox8->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox8_SelectedIndexChanged);
			this->listBox7->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox7_SelectedIndexChanged);
			this->listBox9->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox9_SelectedIndexChanged);
			this->listBox10->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox10_SelectedIndexChanged);
			this->listBox11->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox11_SelectedIndexChanged);
			this->listBox12->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox10_SelectedIndexChanged);

			this->listBox8->DisplayMember = "";
			this->listBox7->DisplayMember = "";
			this->listBox9->DisplayMember = "";
			this->listBox10->DisplayMember = "";
			this->listBox11->DisplayMember = "";
			this->listBox12->DisplayMember = "";
			this->NoPublicadas = gcnew ::ListaCanciones();
			songPaths = gcnew Dictionary<String^, String^>();
			trackCovers = gcnew Dictionary<String^, Image^>();
			this->checkBox3->CheckedChanged += gcnew System::EventHandler(this, &MyForm::checkBox3_CheckedChanged);
			this->btnAgregarAudios->Click += gcnew System::EventHandler(this, &MyForm::btnAgregarAudios_Click);
			this->listBox6->DrawMode = DrawMode::OwnerDrawVariable;
			checkBox3_CheckedChanged(nullptr, nullptr);
			this->single = (this->checkBox3 && this->checkBox3->Checked);
			if (this->textBox7) this->textBox7->Enabled = !this->single;
			songPaths = gcnew System::Collections::Generic::Dictionary<System::String^, System::String^>();
			trackCovers = gcnew System::Collections::Generic::Dictionary<System::String^, System::Drawing::Image^>();

		}
		// fin Init 
		//=====================================================================================// 
	protected:
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
		// Mas codigo Generado 
		//=====================================================================================// 
	private: System::Windows::Forms::Panel^ panel1;
	protected:
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::TextBox^ textBox2;
	private: System::Windows::Forms::LinkLabel^ linkLabel1;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::CheckBox^ checkBox2;
	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::Panel^ PanelRegistroArtista;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label11;
	private: System::Windows::Forms::Label^ label12;
	private: System::Windows::Forms::Label^ label13;
	private: System::Windows::Forms::Label^ label14;
	private: System::Windows::Forms::Button^ button4;
	private: System::Windows::Forms::Button^ button3;
	private: System::Windows::Forms::TextBox^ UsuarioBox;
	private: System::Windows::Forms::TextBox^ Nombre;
	private: System::Windows::Forms::TextBox^ NomArtistico;
	private: System::Windows::Forms::TextBox^ CorreoBox;
	private: System::Windows::Forms::PictureBox^ pictureBox2;
	private: System::Windows::Forms::Button^ button5;
	private: System::Windows::Forms::TextBox^ contra;
	private: System::Windows::Forms::DateTimePicker^ Fecha;
	private: System::Windows::Forms::ComboBox^ Genero;
	private: System::Windows::Forms::TextBox^ valicontra;
	private: System::Windows::Forms::Panel^ PanelRegistroUsuario;
	private: System::Windows::Forms::DateTimePicker^ edad2;
	private: System::Windows::Forms::TextBox^ valicontra2;
	private: System::Windows::Forms::TextBox^ contra2;
	private: System::Windows::Forms::Button^ button6;
	private: System::Windows::Forms::PictureBox^ pictureBox3;
	private: System::Windows::Forms::TextBox^ Nombre2;
	private: System::Windows::Forms::TextBox^ Usuario2;
	private: System::Windows::Forms::Button^ button7;
	private: System::Windows::Forms::Button^ button8;
	private: System::Windows::Forms::Label^ label17;
	private: System::Windows::Forms::Label^ label18;
	private: System::Windows::Forms::Label^ label19;
	private: System::Windows::Forms::Label^ label20;
	private: System::Windows::Forms::Label^ label21;
	private: System::Windows::Forms::Label^ label23;
	private: System::Windows::Forms::Label^ label25;
	private: System::Windows::Forms::Panel^ PanelUsuario;
	private: System::Windows::Forms::PictureBox^ pictureBox6;
	private: System::Windows::Forms::Label^ label22;
	private: System::Windows::Forms::PictureBox^ pictureBox7;
	private: System::Windows::Forms::Label^ label24;
	private: System::Windows::Forms::Label^ label10;
	private: System::Windows::Forms::Label^ label9;
	private: System::Windows::Forms::Label^ label8;
	private: System::Windows::Forms::Label^ label7;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::Panel^ panel3;
	private: System::Windows::Forms::PictureBox^ pictureBox10;
	private: System::Windows::Forms::PictureBox^ pictureBox13;
	private: System::Windows::Forms::PictureBox^ pictureBox11;
	private: System::Windows::Forms::PictureBox^ pictureBox14;
	private: System::Windows::Forms::PictureBox^ pictureBox15;
	private: System::Windows::Forms::PictureBox^ pictureBox16;
	private: System::Windows::Forms::PictureBox^ pictureBox17;
	private: System::Windows::Forms::PictureBox^ pictureBox18;
	private: System::Windows::Forms::PictureBox^ pictureBox12;
	private: System::Windows::Forms::PictureBox^ pictureBox9;
	private: System::Windows::Forms::ComboBox^ PaisBox;
	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::PictureBox^ pictureBox19;
	private: System::Windows::Forms::PictureBox^ pictureBox24;
	private: System::Windows::Forms::PictureBox^ pictureBox25;
	private: System::Windows::Forms::PictureBox^ pictureBox28;
	private: System::Windows::Forms::Panel^ panel5;
	private: System::Windows::Forms::PictureBox^ pictureBox20;
	private: System::Windows::Forms::PictureBox^ pictureBox22;
	private: System::Windows::Forms::PictureBox^ pictureBox23;
	private: System::Windows::Forms::TextBox^ correou;
	private: System::Windows::Forms::Label^ label31;
	private: System::Windows::Forms::PictureBox^ pictureBox21;
	private: System::Windows::Forms::TextBox^ idboxusu;
	private: System::Windows::Forms::Label^ label32;
	private: System::Windows::Forms::PictureBox^ pictureBox26;
	private: System::Windows::Forms::TextBox^ IdboxArt;
	private: System::Windows::Forms::Label^ label34;
	private: System::Windows::Forms::Button^ button31;
	private: System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code

		   void InitializeComponent(void)
		   {
			   System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			   this->panel1 = (gcnew System::Windows::Forms::Panel());
			   this->panel2 = (gcnew System::Windows::Forms::Panel());
			   this->button31 = (gcnew System::Windows::Forms::Button());
			   this->button2 = (gcnew System::Windows::Forms::Button());
			   this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			   this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			   this->label3 = (gcnew System::Windows::Forms::Label());
			   this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			   this->linkLabel1 = (gcnew System::Windows::Forms::LinkLabel());
			   this->label2 = (gcnew System::Windows::Forms::Label());
			   this->button1 = (gcnew System::Windows::Forms::Button());
			   this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			   this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			   this->label1 = (gcnew System::Windows::Forms::Label());
			   this->PanelRegistroArtista = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox26 = (gcnew System::Windows::Forms::PictureBox());
			   this->IdboxArt = (gcnew System::Windows::Forms::TextBox());
			   this->label34 = (gcnew System::Windows::Forms::Label());
			   this->label6 = (gcnew System::Windows::Forms::Label());
			   this->PaisBox = (gcnew System::Windows::Forms::ComboBox());
			   this->pictureBox9 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox12 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox16 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox17 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox15 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox18 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox14 = (gcnew System::Windows::Forms::PictureBox());
			   this->label14 = (gcnew System::Windows::Forms::Label());
			   this->panel3 = (gcnew System::Windows::Forms::Panel());
			   this->Fecha = (gcnew System::Windows::Forms::DateTimePicker());
			   this->pictureBox13 = (gcnew System::Windows::Forms::PictureBox());
			   this->Genero = (gcnew System::Windows::Forms::ComboBox());
			   this->pictureBox11 = (gcnew System::Windows::Forms::PictureBox());
			   this->valicontra = (gcnew System::Windows::Forms::TextBox());
			   this->pictureBox10 = (gcnew System::Windows::Forms::PictureBox());
			   this->contra = (gcnew System::Windows::Forms::TextBox());
			   this->Nombre = (gcnew System::Windows::Forms::TextBox());
			   this->NomArtistico = (gcnew System::Windows::Forms::TextBox());
			   this->CorreoBox = (gcnew System::Windows::Forms::TextBox());
			   this->UsuarioBox = (gcnew System::Windows::Forms::TextBox());
			   this->button4 = (gcnew System::Windows::Forms::Button());
			   this->button3 = (gcnew System::Windows::Forms::Button());
			   this->label11 = (gcnew System::Windows::Forms::Label());
			   this->label12 = (gcnew System::Windows::Forms::Label());
			   this->label13 = (gcnew System::Windows::Forms::Label());
			   this->label9 = (gcnew System::Windows::Forms::Label());
			   this->label8 = (gcnew System::Windows::Forms::Label());
			   this->label7 = (gcnew System::Windows::Forms::Label());
			   this->label5 = (gcnew System::Windows::Forms::Label());
			   this->label4 = (gcnew System::Windows::Forms::Label());
			   this->panel18 = (gcnew System::Windows::Forms::Panel());
			   this->label10 = (gcnew System::Windows::Forms::Label());
			   this->button5 = (gcnew System::Windows::Forms::Button());
			   this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			   this->PanelRegistroUsuario = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox21 = (gcnew System::Windows::Forms::PictureBox());
			   this->idboxusu = (gcnew System::Windows::Forms::TextBox());
			   this->label32 = (gcnew System::Windows::Forms::Label());
			   this->correou = (gcnew System::Windows::Forms::TextBox());
			   this->label31 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox24 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox25 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox28 = (gcnew System::Windows::Forms::PictureBox());
			   this->panel5 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox20 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox22 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox23 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox19 = (gcnew System::Windows::Forms::PictureBox());
			   this->edad2 = (gcnew System::Windows::Forms::DateTimePicker());
			   this->valicontra2 = (gcnew System::Windows::Forms::TextBox());
			   this->contra2 = (gcnew System::Windows::Forms::TextBox());
			   this->Nombre2 = (gcnew System::Windows::Forms::TextBox());
			   this->Usuario2 = (gcnew System::Windows::Forms::TextBox());
			   this->button7 = (gcnew System::Windows::Forms::Button());
			   this->button8 = (gcnew System::Windows::Forms::Button());
			   this->label17 = (gcnew System::Windows::Forms::Label());
			   this->label18 = (gcnew System::Windows::Forms::Label());
			   this->label20 = (gcnew System::Windows::Forms::Label());
			   this->label21 = (gcnew System::Windows::Forms::Label());
			   this->label23 = (gcnew System::Windows::Forms::Label());
			   this->label25 = (gcnew System::Windows::Forms::Label());
			   this->panel19 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			   this->button6 = (gcnew System::Windows::Forms::Button());
			   this->label19 = (gcnew System::Windows::Forms::Label());
			   this->PanelUsuario = (gcnew System::Windows::Forms::Panel());
			   this->PanelCatalogo = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox33 = (gcnew System::Windows::Forms::PictureBox());
			   this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			   this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			   this->Panelstatsusu = (gcnew System::Windows::Forms::Panel());
			   this->dataGridView4 = (gcnew System::Windows::Forms::DataGridView());
			   this->label80 = (gcnew System::Windows::Forms::Label());
			   this->label81 = (gcnew System::Windows::Forms::Label());
			   this->label82 = (gcnew System::Windows::Forms::Label());
			   this->label83 = (gcnew System::Windows::Forms::Label());
			   this->listBox15 = (gcnew System::Windows::Forms::ListBox());
			   this->listBox16 = (gcnew System::Windows::Forms::ListBox());
			   this->listBox18 = (gcnew System::Windows::Forms::ListBox());
			   this->PanelConfig = (gcnew System::Windows::Forms::Panel());
			   this->button12 = (gcnew System::Windows::Forms::Button());
			   this->label39 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox44 = (gcnew System::Windows::Forms::PictureBox());
			   this->textBox9 = (gcnew System::Windows::Forms::TextBox());
			   this->label40 = (gcnew System::Windows::Forms::Label());
			   this->textBox10 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox11 = (gcnew System::Windows::Forms::TextBox());
			   this->button15 = (gcnew System::Windows::Forms::Button());
			   this->textBox12 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox13 = (gcnew System::Windows::Forms::TextBox());
			   this->button17 = (gcnew System::Windows::Forms::Button());
			   this->label42 = (gcnew System::Windows::Forms::Label());
			   this->label43 = (gcnew System::Windows::Forms::Label());
			   this->label44 = (gcnew System::Windows::Forms::Label());
			   this->label45 = (gcnew System::Windows::Forms::Label());
			   this->label46 = (gcnew System::Windows::Forms::Label());
			   this->PanelCrearPlay = (gcnew System::Windows::Forms::Panel());
			   this->label37 = (gcnew System::Windows::Forms::Label());
			   this->label36 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox41 = (gcnew System::Windows::Forms::PictureBox());
			   this->label35 = (gcnew System::Windows::Forms::Label());
			   this->textBox6 = (gcnew System::Windows::Forms::TextBox());
			   this->listBox4 = (gcnew System::Windows::Forms::ListBox());
			   this->pictureBox40 = (gcnew System::Windows::Forms::PictureBox());
			   this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			   this->PanelFav = (gcnew System::Windows::Forms::Panel());
			   this->listBox5 = (gcnew System::Windows::Forms::ListBox());
			   this->pictureBox43 = (gcnew System::Windows::Forms::PictureBox());
			   this->textBox8 = (gcnew System::Windows::Forms::TextBox());
			   this->PanelVerPlay = (gcnew System::Windows::Forms::Panel());
			   this->listBox3 = (gcnew System::Windows::Forms::ListBox());
			   this->pictureBox39 = (gcnew System::Windows::Forms::PictureBox());
			   this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			   this->listBox2 = (gcnew System::Windows::Forms::ListBox());
			   this->panel14 = (gcnew System::Windows::Forms::Panel());
			   this->axWindowsMediaPlayer2 = (gcnew AxWMPLib::AxWindowsMediaPlayer());
			   this->pictureBox38 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox37 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox36 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox35 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox34 = (gcnew System::Windows::Forms::PictureBox());
			   this->label33 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox32 = (gcnew System::Windows::Forms::PictureBox());
			   this->panel11 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox64 = (gcnew System::Windows::Forms::PictureBox());
			   this->label38 = (gcnew System::Windows::Forms::Label());
			   this->label73 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox42 = (gcnew System::Windows::Forms::PictureBox());
			   this->label26 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox31 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox30 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox29 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox27 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox8 = (gcnew System::Windows::Forms::PictureBox());
			   this->label30 = (gcnew System::Windows::Forms::Label());
			   this->label29 = (gcnew System::Windows::Forms::Label());
			   this->label28 = (gcnew System::Windows::Forms::Label());
			   this->label27 = (gcnew System::Windows::Forms::Label());
			   this->panel12 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox6 = (gcnew System::Windows::Forms::PictureBox());
			   this->label22 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox7 = (gcnew System::Windows::Forms::PictureBox());
			   this->label24 = (gcnew System::Windows::Forms::Label());
			   this->panelArtista = (gcnew System::Windows::Forms::Panel());
			   this->PanelConfi = (gcnew System::Windows::Forms::Panel());
			   this->panel15 = (gcnew System::Windows::Forms::Panel());
			   this->panel13 = (gcnew System::Windows::Forms::Panel());
			   this->label94 = (gcnew System::Windows::Forms::Label());
			   this->panel10 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox63 = (gcnew System::Windows::Forms::PictureBox());
			   this->button9 = (gcnew System::Windows::Forms::Button());
			   this->label70 = (gcnew System::Windows::Forms::Label());
			   this->label71 = (gcnew System::Windows::Forms::Label());
			   this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			   this->textBox21 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox22 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox23 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox24 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox25 = (gcnew System::Windows::Forms::TextBox());
			   this->textBox26 = (gcnew System::Windows::Forms::TextBox());
			   this->button10 = (gcnew System::Windows::Forms::Button());
			   this->button11 = (gcnew System::Windows::Forms::Button());
			   this->label72 = (gcnew System::Windows::Forms::Label());
			   this->label76 = (gcnew System::Windows::Forms::Label());
			   this->label77 = (gcnew System::Windows::Forms::Label());
			   this->label78 = (gcnew System::Windows::Forms::Label());
			   this->label79 = (gcnew System::Windows::Forms::Label());
			   this->panelStats = (gcnew System::Windows::Forms::Panel());
			   this->label104 = (gcnew System::Windows::Forms::Label());
			   this->label66 = (gcnew System::Windows::Forms::Label());
			   this->label65 = (gcnew System::Windows::Forms::Label());
			   this->label60 = (gcnew System::Windows::Forms::Label());
			   this->listBox17 = (gcnew System::Windows::Forms::ListBox());
			   this->listBox14 = (gcnew System::Windows::Forms::ListBox());
			   this->listBox13 = (gcnew System::Windows::Forms::ListBox());
			   this->panel24 = (gcnew System::Windows::Forms::Panel());
			   this->dataGridView2 = (gcnew System::Windows::Forms::DataGridView());
			   this->label67 = (gcnew System::Windows::Forms::Label());
			   this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			   this->label68 = (gcnew System::Windows::Forms::Label());
			   this->PanelSubir = (gcnew System::Windows::Forms::Panel());
			   this->label58 = (gcnew System::Windows::Forms::Label());
			   this->textBox14 = (gcnew System::Windows::Forms::TextBox());
			   this->label47 = (gcnew System::Windows::Forms::Label());
			   this->lblConteoArchivos = (gcnew System::Windows::Forms::Label());
			   this->btnAgregarAudios = (gcnew System::Windows::Forms::Label());
			   this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
			   this->panel17 = (gcnew System::Windows::Forms::Panel());
			   this->label96 = (gcnew System::Windows::Forms::Label());
			   this->panel16 = (gcnew System::Windows::Forms::Panel());
			   this->label97 = (gcnew System::Windows::Forms::Label());
			   this->listBox6 = (gcnew System::Windows::Forms::ListBox());
			   this->label95 = (gcnew System::Windows::Forms::Label());
			   this->label48 = (gcnew System::Windows::Forms::Label());
			   this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
			   this->label86 = (gcnew System::Windows::Forms::Label());
			   this->textBox15 = (gcnew System::Windows::Forms::TextBox());
			   this->label85 = (gcnew System::Windows::Forms::Label());
			   this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			   this->label84 = (gcnew System::Windows::Forms::Label());
			   this->textBox7 = (gcnew System::Windows::Forms::TextBox());
			   this->label49 = (gcnew System::Windows::Forms::Label());
			   this->label41 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox58 = (gcnew System::Windows::Forms::PictureBox());
			   this->PanelEliminar = (gcnew System::Windows::Forms::Panel());
			   this->label101 = (gcnew System::Windows::Forms::Label());
			   this->label100 = (gcnew System::Windows::Forms::Label());
			   this->label99 = (gcnew System::Windows::Forms::Label());
			   this->listBox9 = (gcnew System::Windows::Forms::ListBox());
			   this->label64 = (gcnew System::Windows::Forms::Label());
			   this->listBox10 = (gcnew System::Windows::Forms::ListBox());
			   this->panel22 = (gcnew System::Windows::Forms::Panel());
			   this->panelDisco = (gcnew System::Windows::Forms::Panel());
			   this->label103 = (gcnew System::Windows::Forms::Label());
			   this->label102 = (gcnew System::Windows::Forms::Label());
			   this->listBox11 = (gcnew System::Windows::Forms::ListBox());
			   this->listBox12 = (gcnew System::Windows::Forms::ListBox());
			   this->panel23 = (gcnew System::Windows::Forms::Panel());
			   this->PanelEditar = (gcnew System::Windows::Forms::Panel());
			   this->label74 = (gcnew System::Windows::Forms::Label());
			   this->comboBox4 = (gcnew System::Windows::Forms::ComboBox());
			   this->label87 = (gcnew System::Windows::Forms::Label());
			   this->textBox28 = (gcnew System::Windows::Forms::TextBox());
			   this->label88 = (gcnew System::Windows::Forms::Label());
			   this->comboBox5 = (gcnew System::Windows::Forms::ComboBox());
			   this->label89 = (gcnew System::Windows::Forms::Label());
			   this->textBox17 = (gcnew System::Windows::Forms::TextBox());
			   this->label63 = (gcnew System::Windows::Forms::Label());
			   this->panel20 = (gcnew System::Windows::Forms::Panel());
			   this->label91 = (gcnew System::Windows::Forms::Label());
			   this->listBox8 = (gcnew System::Windows::Forms::ListBox());
			   this->label90 = (gcnew System::Windows::Forms::Label());
			   this->listBox7 = (gcnew System::Windows::Forms::ListBox());
			   this->panel21 = (gcnew System::Windows::Forms::Panel());
			   this->label98 = (gcnew System::Windows::Forms::Label());
			   this->label61 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox60 = (gcnew System::Windows::Forms::PictureBox());
			   this->label62 = (gcnew System::Windows::Forms::Label());
			   this->panel6 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox50 = (gcnew System::Windows::Forms::PictureBox());
			   this->label15 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox4 = (gcnew System::Windows::Forms::PictureBox());
			   this->label51 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox5 = (gcnew System::Windows::Forms::PictureBox());
			   this->label52 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox45 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox46 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox47 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox48 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox49 = (gcnew System::Windows::Forms::PictureBox());
			   this->label53 = (gcnew System::Windows::Forms::Label());
			   this->label54 = (gcnew System::Windows::Forms::Label());
			   this->label55 = (gcnew System::Windows::Forms::Label());
			   this->label56 = (gcnew System::Windows::Forms::Label());
			   this->panel7 = (gcnew System::Windows::Forms::Panel());
			   this->panel8 = (gcnew System::Windows::Forms::Panel());
			   this->pictureBox56 = (gcnew System::Windows::Forms::PictureBox());
			   this->label75 = (gcnew System::Windows::Forms::Label());
			   this->progressBar2 = (gcnew System::Windows::Forms::ProgressBar());
			   this->pictureBox54 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox52 = (gcnew System::Windows::Forms::PictureBox());
			   this->label93 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox51 = (gcnew System::Windows::Forms::PictureBox());
			   this->pictureBox53 = (gcnew System::Windows::Forms::PictureBox());
			   this->label57 = (gcnew System::Windows::Forms::Label());
			   this->pictureBox57 = (gcnew System::Windows::Forms::PictureBox());
			   this->label59 = (gcnew System::Windows::Forms::Label());
			   this->label16 = (gcnew System::Windows::Forms::Label());
			   this->panel1->SuspendLayout();
			   this->panel2->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			   this->PanelRegistroArtista->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox26))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox9))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox12))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox16))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox17))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox15))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox18))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox14))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox13))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox11))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox10))->BeginInit();
			   this->panel18->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
			   this->PanelRegistroUsuario->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox21))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox24))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox25))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox28))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox20))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox22))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox23))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox19))->BeginInit();
			   this->panel19->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->BeginInit();
			   this->PanelUsuario->SuspendLayout();
			   this->PanelCatalogo->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox33))->BeginInit();
			   this->Panelstatsusu->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView4))->BeginInit();
			   this->PanelConfig->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox44))->BeginInit();
			   this->PanelCrearPlay->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox41))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox40))->BeginInit();
			   this->PanelFav->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox43))->BeginInit();
			   this->PanelVerPlay->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox39))->BeginInit();
			   this->panel14->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->axWindowsMediaPlayer2))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox38))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox37))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox36))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox35))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox34))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox32))->BeginInit();
			   this->panel11->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox64))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox42))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox31))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox30))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox29))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox27))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox8))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox6))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox7))->BeginInit();
			   this->panelArtista->SuspendLayout();
			   this->PanelConfi->SuspendLayout();
			   this->panel13->SuspendLayout();
			   this->panel10->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox63))->BeginInit();
			   this->panelStats->SuspendLayout();
			   this->panel24->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView2))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			   this->PanelSubir->SuspendLayout();
			   this->panel16->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox58))->BeginInit();
			   this->PanelEliminar->SuspendLayout();
			   this->panelDisco->SuspendLayout();
			   this->PanelEditar->SuspendLayout();
			   this->panel20->SuspendLayout();
			   this->panel21->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox60))->BeginInit();
			   this->panel6->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox50))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox5))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox45))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox46))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox47))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox48))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox49))->BeginInit();
			   this->panel8->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox56))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox54))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox52))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox51))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox53))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox57))->BeginInit();
			   this->SuspendLayout();
			   // 
			   // panel1
			   // 
			   this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(12)), static_cast<System::Int32>(static_cast<System::Byte>(12)),
				   static_cast<System::Int32>(static_cast<System::Byte>(12)));
			   this->panel1->Controls->Add(this->panel2);
			   this->panel1->Controls->Add(this->textBox2);
			   this->panel1->Controls->Add(this->linkLabel1);
			   this->panel1->Controls->Add(this->label2);
			   this->panel1->Controls->Add(this->button1);
			   this->panel1->Controls->Add(this->textBox1);
			   this->panel1->Controls->Add(this->pictureBox1);
			   this->panel1->Controls->Add(this->label1);
			   this->panel1->Location = System::Drawing::Point(0, 0);
			   this->panel1->Name = L"panel1";
			   this->panel1->Size = System::Drawing::Size(1396, 736);
			   this->panel1->TabIndex = 0;
			   // 
			   // panel2
			   // 
			   this->panel2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(12)), static_cast<System::Int32>(static_cast<System::Byte>(12)),
				   static_cast<System::Int32>(static_cast<System::Byte>(12)));
			   this->panel2->Controls->Add(this->button31);
			   this->panel2->Controls->Add(this->button2);
			   this->panel2->Controls->Add(this->checkBox2);
			   this->panel2->Controls->Add(this->checkBox1);
			   this->panel2->Controls->Add(this->label3);
			   this->panel2->Location = System::Drawing::Point(575, 240);
			   this->panel2->Name = L"panel2";
			   this->panel2->Size = System::Drawing::Size(260, 215);
			   this->panel2->TabIndex = 8;
			   // 
			   // button31
			   // 
			   this->button31->BackColor = System::Drawing::Color::Gainsboro;
			   this->button31->FlatAppearance->BorderSize = 0;
			   this->button31->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button31->ForeColor = System::Drawing::Color::Black;
			   this->button31->Location = System::Drawing::Point(143, 153);
			   this->button31->Name = L"button31";
			   this->button31->Size = System::Drawing::Size(92, 23);
			   this->button31->TabIndex = 13;
			   this->button31->Text = L"Cancelar";
			   this->button31->UseVisualStyleBackColor = false;
			   this->button31->Click += gcnew System::EventHandler(this, &MyForm::button31_Click);
			   // 
			   // button2
			   // 
			   this->button2->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button2->FlatAppearance->BorderSize = 0;
			   this->button2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button2->ForeColor = System::Drawing::Color::Transparent;
			   this->button2->Location = System::Drawing::Point(27, 153);
			   this->button2->Name = L"button2";
			   this->button2->Size = System::Drawing::Size(92, 23);
			   this->button2->TabIndex = 12;
			   this->button2->Text = L"Aceptar";
			   this->button2->UseVisualStyleBackColor = false;
			   this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			   // 
			   // checkBox2
			   // 
			   this->checkBox2->AutoSize = true;
			   this->checkBox2->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->checkBox2->Location = System::Drawing::Point(165, 83);
			   this->checkBox2->Name = L"checkBox2";
			   this->checkBox2->Size = System::Drawing::Size(40, 17);
			   this->checkBox2->TabIndex = 11;
			   this->checkBox2->Text = L"No";
			   this->checkBox2->UseVisualStyleBackColor = false;
			   // 
			   // checkBox1
			   // 
			   this->checkBox1->AutoSize = true;
			   this->checkBox1->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->checkBox1->Location = System::Drawing::Point(59, 83);
			   this->checkBox1->Name = L"checkBox1";
			   this->checkBox1->Size = System::Drawing::Size(35, 17);
			   this->checkBox1->TabIndex = 10;
			   this->checkBox1->Text = L"Si";
			   this->checkBox1->UseVisualStyleBackColor = false;
			   // 
			   // label3
			   // 
			   this->label3->AutoSize = true;
			   this->label3->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label3->ForeColor = System::Drawing::Color::White;
			   this->label3->Location = System::Drawing::Point(54, 25);
			   this->label3->Name = L"label3";
			   this->label3->RightToLeft = System::Windows::Forms::RightToLeft::No;
			   this->label3->Size = System::Drawing::Size(151, 25);
			   this->label3->TabIndex = 9;
			   this->label3->Text = L"¿Eres un Artista\?";
			   // 
			   // textBox2
			   // 
			   this->textBox2->BackColor = System::Drawing::Color::LightGray;
			   this->textBox2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox2->ForeColor = System::Drawing::Color::Black;
			   this->textBox2->Location = System::Drawing::Point(575, 329);
			   this->textBox2->Multiline = true;
			   this->textBox2->Name = L"textBox2";
			   this->textBox2->PasswordChar = '*';
			   this->textBox2->Size = System::Drawing::Size(260, 43);
			   this->textBox2->TabIndex = 5;
			   this->textBox2->Text = L"Contraseña";
			   this->textBox2->UseSystemPasswordChar = true;
			   this->textBox2->MouseEnter += gcnew System::EventHandler(this, &MyForm::textBox2_MouseEnter);
			   this->textBox2->MouseLeave += gcnew System::EventHandler(this, &MyForm::textBox2_MouseLeave_1);
			   // 
			   // linkLabel1
			   // 
			   this->linkLabel1->AutoSize = true;
			   this->linkLabel1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->linkLabel1->LinkColor = System::Drawing::Color::PaleVioletRed;
			   this->linkLabel1->Location = System::Drawing::Point(639, 522);
			   this->linkLabel1->Name = L"linkLabel1";
			   this->linkLabel1->Size = System::Drawing::Size(123, 25);
			   this->linkLabel1->TabIndex = 7;
			   this->linkLabel1->TabStop = true;
			   this->linkLabel1->Text = L"Crear Cuenta";
			   this->linkLabel1->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &MyForm::linkLabel1_LinkClicked);
			   this->linkLabel1->MouseEnter += gcnew System::EventHandler(this, &MyForm::linkLabel1_MouseEnter);
			   // 
			   // label2
			   // 
			   this->label2->AutoSize = true;
			   this->label2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label2->ForeColor = System::Drawing::Color::White;
			   this->label2->Location = System::Drawing::Point(595, 482);
			   this->label2->Name = L"label2";
			   this->label2->RightToLeft = System::Windows::Forms::RightToLeft::No;
			   this->label2->Size = System::Drawing::Size(209, 25);
			   this->label2->TabIndex = 6;
			   this->label2->Text = L"¿No tienes una cuenta\?";
			   // 
			   // button1
			   // 
			   this->button1->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button1->FlatAppearance->BorderSize = 0;
			   this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button1->ForeColor = System::Drawing::Color::White;
			   this->button1->Location = System::Drawing::Point(575, 412);
			   this->button1->Name = L"button1";
			   this->button1->Size = System::Drawing::Size(260, 43);
			   this->button1->TabIndex = 4;
			   this->button1->Text = L"Iniciar Sesion";
			   this->button1->UseVisualStyleBackColor = false;
			   this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			   // 
			   // textBox1
			   // 
			   this->textBox1->BackColor = System::Drawing::Color::LightGray;
			   this->textBox1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox1->ForeColor = System::Drawing::Color::Black;
			   this->textBox1->Location = System::Drawing::Point(575, 252);
			   this->textBox1->Multiline = true;
			   this->textBox1->Name = L"textBox1";
			   this->textBox1->Size = System::Drawing::Size(260, 43);
			   this->textBox1->TabIndex = 2;
			   this->textBox1->Text = L"Nombre de Usuario";
			   this->textBox1->MouseEnter += gcnew System::EventHandler(this, &MyForm::textBox1_MouseEnter);
			   this->textBox1->MouseLeave += gcnew System::EventHandler(this, &MyForm::textBox1_MouseLeave);
			   // 
			   // pictureBox1
			   // 
			   this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			   this->pictureBox1->Location = System::Drawing::Point(575, 161);
			   this->pictureBox1->Name = L"pictureBox1";
			   this->pictureBox1->Size = System::Drawing::Size(63, 52);
			   this->pictureBox1->TabIndex = 1;
			   this->pictureBox1->TabStop = false;
			   // 
			   // label1
			   // 
			   this->label1->AutoSize = true;
			   this->label1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 36, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label1->ForeColor = System::Drawing::Color::White;
			   this->label1->Location = System::Drawing::Point(633, 148);
			   this->label1->Name = L"label1";
			   this->label1->Size = System::Drawing::Size(202, 65);
			   this->label1->TabIndex = 0;
			   this->label1->Text = L"TuneUp";
			   // 
			   // PanelRegistroArtista
			   // 
			   this->PanelRegistroArtista->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(12)),
				   static_cast<System::Int32>(static_cast<System::Byte>(12)), static_cast<System::Int32>(static_cast<System::Byte>(12)));
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox26);
			   this->PanelRegistroArtista->Controls->Add(this->IdboxArt);
			   this->PanelRegistroArtista->Controls->Add(this->label34);
			   this->PanelRegistroArtista->Controls->Add(this->label6);
			   this->PanelRegistroArtista->Controls->Add(this->PaisBox);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox9);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox12);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox16);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox17);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox15);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox18);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox14);
			   this->PanelRegistroArtista->Controls->Add(this->label14);
			   this->PanelRegistroArtista->Controls->Add(this->panel3);
			   this->PanelRegistroArtista->Controls->Add(this->Fecha);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox13);
			   this->PanelRegistroArtista->Controls->Add(this->Genero);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox11);
			   this->PanelRegistroArtista->Controls->Add(this->valicontra);
			   this->PanelRegistroArtista->Controls->Add(this->pictureBox10);
			   this->PanelRegistroArtista->Controls->Add(this->contra);
			   this->PanelRegistroArtista->Controls->Add(this->Nombre);
			   this->PanelRegistroArtista->Controls->Add(this->NomArtistico);
			   this->PanelRegistroArtista->Controls->Add(this->CorreoBox);
			   this->PanelRegistroArtista->Controls->Add(this->UsuarioBox);
			   this->PanelRegistroArtista->Controls->Add(this->button4);
			   this->PanelRegistroArtista->Controls->Add(this->button3);
			   this->PanelRegistroArtista->Controls->Add(this->label11);
			   this->PanelRegistroArtista->Controls->Add(this->label12);
			   this->PanelRegistroArtista->Controls->Add(this->label13);
			   this->PanelRegistroArtista->Controls->Add(this->label9);
			   this->PanelRegistroArtista->Controls->Add(this->label8);
			   this->PanelRegistroArtista->Controls->Add(this->label7);
			   this->PanelRegistroArtista->Controls->Add(this->label5);
			   this->PanelRegistroArtista->Controls->Add(this->label4);
			   this->PanelRegistroArtista->Controls->Add(this->panel18);
			   this->PanelRegistroArtista->Location = System::Drawing::Point(0, 0);
			   this->PanelRegistroArtista->Margin = System::Windows::Forms::Padding(2);
			   this->PanelRegistroArtista->Name = L"PanelRegistroArtista";
			   this->PanelRegistroArtista->Size = System::Drawing::Size(1394, 736);
			   this->PanelRegistroArtista->TabIndex = 9;
			   // 
			   // pictureBox26
			   // 
			   this->pictureBox26->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox26->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox26.Image")));
			   this->pictureBox26->Location = System::Drawing::Point(81, 438);
			   this->pictureBox26->Name = L"pictureBox26";
			   this->pictureBox26->Size = System::Drawing::Size(63, 47);
			   this->pictureBox26->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox26->TabIndex = 60;
			   this->pictureBox26->TabStop = false;
			   // 
			   // IdboxArt
			   // 
			   this->IdboxArt->Enabled = false;
			   this->IdboxArt->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->IdboxArt->Location = System::Drawing::Point(149, 459);
			   this->IdboxArt->Margin = System::Windows::Forms::Padding(2);
			   this->IdboxArt->Name = L"IdboxArt";
			   this->IdboxArt->Size = System::Drawing::Size(272, 26);
			   this->IdboxArt->TabIndex = 59;
			   // 
			   // label34
			   // 
			   this->label34->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label34->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label34->ForeColor = System::Drawing::Color::White;
			   this->label34->Location = System::Drawing::Point(149, 440);
			   this->label34->Name = L"label34";
			   this->label34->Size = System::Drawing::Size(272, 19);
			   this->label34->TabIndex = 58;
			   this->label34->Text = L"Id: ";
			   // 
			   // label6
			   // 
			   this->label6->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label6->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label6->ForeColor = System::Drawing::Color::White;
			   this->label6->Location = System::Drawing::Point(149, 183);
			   this->label6->Name = L"label6";
			   this->label6->Size = System::Drawing::Size(270, 23);
			   this->label6->TabIndex = 41;
			   this->label6->Text = L"Usuario:";
			   // 
			   // PaisBox
			   // 
			   this->PaisBox->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->PaisBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->PaisBox->FormattingEnabled = true;
			   this->PaisBox->Items->AddRange(gcnew cli::array< System::Object^  >(189) {
				   L"Afganistán", L"Albania", L"Alemania", L"Andorra",
					   L"Angola", L"Antigua y Barbuda", L"Arabia Saudita", L"Argentina", L"Armenia", L"Australia", L"Austria", L"Azerbaiyán", L"Bahamas",
					   L"Baréin", L"Bangladés", L"Barbados", L"Bélgica", L"Belice", L"Benín", L"Bielorrusia", L"Birmania (Myanmar)", L"Bolivia", L"Bosnia y Herzegovina",
					   L"Botsuana", L"Brasil", L"Brunéi", L"Bulgaria", L"Burkina Faso", L"Burundi", L"Cabo Verde", L"Camboya", L"Camerún", L"Canadá",
					   L"Catar", L"Chad", L"Chile", L"China", L"Chipre", L"Colombia", L"Comoras", L"República del Congo", L"República Democrática del Congo",
					   L"Corea del Norte", L"Corea del Sur", L"Costa de Marfil", L"Costa Rica", L"Croacia", L"Cuba", L"República Centroafricana", L"República Checa",
					   L"República Dominicana", L"Dinamarca", L"Yibuti", L"Dominica", L"Ecuador", L"Egipto", L"El Salvador", L"Emiratos Árabes Unidos",
					   L"Eritrea", L"Eslovaquia", L"Eslovenia", L"España", L"Estados Unidos", L"Estonia", L"Esuatini", L"Etiopía", L"Filipinas", L"Finlandia",
					   L"Fiyi", L"Francia", L"Gabón", L"Gambia", L"Georgia", L"Ghana", L"Grecia", L"Granada", L"Guatemala", L"Guinea", L"Guinea-Bisáu",
					   L"Guinea Ecuatorial", L"Guyana", L"Haití", L"Honduras", L"Hungría", L"India", L"Indonesia", L"Irán", L"Iraq", L"Irlanda", L"Islandia",
					   L"Israel", L"Italia", L"Jamaica", L"Japón", L"Jordania", L"Kazajistán", L"Kenia", L"Kirguistán", L"Kiribati", L"Kuwait", L"Laos",
					   L"Letonia", L"Líbano", L"Liberia", L"Libia", L"Liechtenstein", L"Lituania", L"Luxemburgo", L"Macedonia del Norte", L"Madagascar",
					   L"Malasia", L"Malaui", L"Maldivas", L"Malí", L"Malta", L"Islas Marshall", L"Mauricio", L"Mauritania", L"México", L"Micronesia",
					   L"Mónaco", L"Mongolia", L"Montenegro", L"Marruecos", L"Mozambique", L"Namibia", L"Nauru", L"Nepal", L"Nueva Zelanda", L"Nicaragua",
					   L"Níger", L"Nigeria", L"Noruega", L"Omán", L"Países Bajos", L"Pakistán", L"Palaos", L"Panamá", L"Papúa Nueva Guinea", L"Paraguay",
					   L"Perú", L"Polonia", L"Portugal", L"Reino Unido", L"Ruanda", L"Rumanía", L"Rusia", L"San Cristóbal y Nieves", L"San Marino",
					   L"San Vicente y las Granadinas", L"Santa Lucía", L"Santo Tomé y Príncipe", L"Senegal", L"Serbia", L"Seychelles", L"Sierra Leona",
					   L"Singapur", L"Siria", L"Somalia", L"Sri Lanka", L"Sudáfrica", L"Sudán", L"Sudán del Sur", L"Suecia", L"Suiza", L"Surinam", L"Tailandia",
					   L"Tanzania", L"Tayikistán", L"Timor-Leste", L"Togo", L"Tonga", L"Trinidad y Tobago", L"Túnez", L"Turquía", L"Turkmenistán", L"Tuvalu",
					   L"Uganda", L"Ucrania", L"Uruguay", L"Uzbekistán", L"Vanuatu", L"Venezuela", L"Vietnam", L"Yemen", L"Zambia", L"Zimbabue", L"Palestina",
					   L"Santa Sede (Ciudad del Vaticano)"
			   });
			   this->PaisBox->Location = System::Drawing::Point(588, 401);
			   this->PaisBox->Margin = System::Windows::Forms::Padding(2);
			   this->PaisBox->Name = L"PaisBox";
			   this->PaisBox->Size = System::Drawing::Size(270, 21);
			   this->PaisBox->TabIndex = 40;
			   // 
			   // pictureBox9
			   // 
			   this->pictureBox9->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox9->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox9.Image")));
			   this->pictureBox9->Location = System::Drawing::Point(519, 249);
			   this->pictureBox9->Name = L"pictureBox9";
			   this->pictureBox9->Size = System::Drawing::Size(63, 48);
			   this->pictureBox9->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox9->TabIndex = 39;
			   this->pictureBox9->TabStop = false;
			   // 
			   // pictureBox12
			   // 
			   this->pictureBox12->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox12->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox12.Image")));
			   this->pictureBox12->Location = System::Drawing::Point(80, 375);
			   this->pictureBox12->Name = L"pictureBox12";
			   this->pictureBox12->Size = System::Drawing::Size(63, 46);
			   this->pictureBox12->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox12->TabIndex = 38;
			   this->pictureBox12->TabStop = false;
			   // 
			   // pictureBox16
			   // 
			   this->pictureBox16->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox16->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox16.Image")));
			   this->pictureBox16->Location = System::Drawing::Point(519, 183);
			   this->pictureBox16->Name = L"pictureBox16";
			   this->pictureBox16->Size = System::Drawing::Size(63, 48);
			   this->pictureBox16->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox16->TabIndex = 34;
			   this->pictureBox16->TabStop = false;
			   // 
			   // pictureBox17
			   // 
			   this->pictureBox17->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox17->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox17.Image")));
			   this->pictureBox17->Location = System::Drawing::Point(520, 373);
			   this->pictureBox17->Name = L"pictureBox17";
			   this->pictureBox17->Size = System::Drawing::Size(63, 48);
			   this->pictureBox17->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox17->TabIndex = 33;
			   this->pictureBox17->TabStop = false;
			   // 
			   // pictureBox15
			   // 
			   this->pictureBox15->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox15->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox15.Image")));
			   this->pictureBox15->Location = System::Drawing::Point(520, 440);
			   this->pictureBox15->Name = L"pictureBox15";
			   this->pictureBox15->Size = System::Drawing::Size(63, 48);
			   this->pictureBox15->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox15->TabIndex = 35;
			   this->pictureBox15->TabStop = false;
			   // 
			   // pictureBox18
			   // 
			   this->pictureBox18->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox18->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox18.Image")));
			   this->pictureBox18->Location = System::Drawing::Point(519, 310);
			   this->pictureBox18->Name = L"pictureBox18";
			   this->pictureBox18->Size = System::Drawing::Size(63, 47);
			   this->pictureBox18->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox18->TabIndex = 37;
			   this->pictureBox18->TabStop = false;
			   // 
			   // pictureBox14
			   // 
			   this->pictureBox14->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox14.Image")));
			   this->pictureBox14->Location = System::Drawing::Point(467, 59);
			   this->pictureBox14->Name = L"pictureBox14";
			   this->pictureBox14->Size = System::Drawing::Size(63, 47);
			   this->pictureBox14->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox14->TabIndex = 36;
			   this->pictureBox14->TabStop = false;
			   // 
			   // label14
			   // 
			   this->label14->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label14->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label14->ForeColor = System::Drawing::Color::White;
			   this->label14->Location = System::Drawing::Point(587, 251);
			   this->label14->Name = L"label14";
			   this->label14->Size = System::Drawing::Size(270, 24);
			   this->label14->TabIndex = 8;
			   this->label14->Text = L"Confirmar:                                                     ";
			   // 
			   // panel3
			   // 
			   this->panel3->BackColor = System::Drawing::Color::LightGray;
			   this->panel3->Location = System::Drawing::Point(463, 183);
			   this->panel3->Name = L"panel3";
			   this->panel3->Size = System::Drawing::Size(20, 457);
			   this->panel3->TabIndex = 27;
			   // 
			   // Fecha
			   // 
			   this->Fecha->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->Fecha->Location = System::Drawing::Point(588, 334);
			   this->Fecha->Margin = System::Windows::Forms::Padding(2);
			   this->Fecha->Name = L"Fecha";
			   this->Fecha->Size = System::Drawing::Size(270, 23);
			   this->Fecha->TabIndex = 26;
			   // 
			   // pictureBox13
			   // 
			   this->pictureBox13->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox13->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox13.Image")));
			   this->pictureBox13->Location = System::Drawing::Point(80, 312);
			   this->pictureBox13->Name = L"pictureBox13";
			   this->pictureBox13->Size = System::Drawing::Size(64, 48);
			   this->pictureBox13->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox13->TabIndex = 32;
			   this->pictureBox13->TabStop = false;
			   // 
			   // Genero
			   // 
			   this->Genero->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->Genero->FormattingEnabled = true;
			   this->Genero->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				   L"pop", L"Corridos", L"cristianos", L"electrónica",
					   L"reguetón", L"rock", L"clasicas"
			   });
			   this->Genero->Location = System::Drawing::Point(588, 465);
			   this->Genero->Margin = System::Windows::Forms::Padding(2);
			   this->Genero->Name = L"Genero";
			   this->Genero->Size = System::Drawing::Size(270, 21);
			   this->Genero->TabIndex = 25;
			   // 
			   // pictureBox11
			   // 
			   this->pictureBox11->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox11->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->pictureBox11->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox11.Image")));
			   this->pictureBox11->Location = System::Drawing::Point(81, 249);
			   this->pictureBox11->Name = L"pictureBox11";
			   this->pictureBox11->Size = System::Drawing::Size(63, 48);
			   this->pictureBox11->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox11->TabIndex = 30;
			   this->pictureBox11->TabStop = false;
			   this->pictureBox11->UseWaitCursor = true;
			   // 
			   // valicontra
			   // 
			   this->valicontra->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->valicontra->Location = System::Drawing::Point(587, 273);
			   this->valicontra->Margin = System::Windows::Forms::Padding(2);
			   this->valicontra->Name = L"valicontra";
			   this->valicontra->PasswordChar = '*';
			   this->valicontra->Size = System::Drawing::Size(270, 26);
			   this->valicontra->TabIndex = 22;
			   this->valicontra->UseSystemPasswordChar = true;
			   // 
			   // pictureBox10
			   // 
			   this->pictureBox10->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox10->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox10.Image")));
			   this->pictureBox10->Location = System::Drawing::Point(80, 183);
			   this->pictureBox10->Name = L"pictureBox10";
			   this->pictureBox10->Size = System::Drawing::Size(64, 49);
			   this->pictureBox10->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox10->TabIndex = 29;
			   this->pictureBox10->TabStop = false;
			   // 
			   // contra
			   // 
			   this->contra->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->contra->Location = System::Drawing::Point(587, 205);
			   this->contra->Margin = System::Windows::Forms::Padding(2);
			   this->contra->Name = L"contra";
			   this->contra->PasswordChar = '*';
			   this->contra->Size = System::Drawing::Size(270, 26);
			   this->contra->TabIndex = 21;
			   this->contra->UseSystemPasswordChar = true;
			   // 
			   // Nombre
			   // 
			   this->Nombre->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->Nombre->Location = System::Drawing::Point(149, 395);
			   this->Nombre->Margin = System::Windows::Forms::Padding(2);
			   this->Nombre->Name = L"Nombre";
			   this->Nombre->Size = System::Drawing::Size(270, 26);
			   this->Nombre->TabIndex = 18;
			   // 
			   // NomArtistico
			   // 
			   this->NomArtistico->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->NomArtistico->Location = System::Drawing::Point(150, 334);
			   this->NomArtistico->Margin = System::Windows::Forms::Padding(2);
			   this->NomArtistico->Name = L"NomArtistico";
			   this->NomArtistico->Size = System::Drawing::Size(270, 26);
			   this->NomArtistico->TabIndex = 17;
			   // 
			   // CorreoBox
			   // 
			   this->CorreoBox->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->CorreoBox->Location = System::Drawing::Point(149, 270);
			   this->CorreoBox->Margin = System::Windows::Forms::Padding(2);
			   this->CorreoBox->Name = L"CorreoBox";
			   this->CorreoBox->Size = System::Drawing::Size(270, 26);
			   this->CorreoBox->TabIndex = 16;
			   // 
			   // UsuarioBox
			   // 
			   this->UsuarioBox->BackColor = System::Drawing::Color::White;
			   this->UsuarioBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->UsuarioBox->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->UsuarioBox->ForeColor = System::Drawing::Color::Black;
			   this->UsuarioBox->Location = System::Drawing::Point(149, 206);
			   this->UsuarioBox->Margin = System::Windows::Forms::Padding(2);
			   this->UsuarioBox->Multiline = true;
			   this->UsuarioBox->Name = L"UsuarioBox";
			   this->UsuarioBox->Size = System::Drawing::Size(270, 26);
			   this->UsuarioBox->TabIndex = 15;
			   // 
			   // button4
			   // 
			   this->button4->BackColor = System::Drawing::Color::LightGray;
			   this->button4->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button4->FlatAppearance->BorderSize = 0;
			   this->button4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button4->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button4->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->button4->Location = System::Drawing::Point(520, 598);
			   this->button4->Margin = System::Windows::Forms::Padding(2);
			   this->button4->Name = L"button4";
			   this->button4->Padding = System::Windows::Forms::Padding(10, 5, 10, 5);
			   this->button4->Size = System::Drawing::Size(340, 42);
			   this->button4->TabIndex = 14;
			   this->button4->Text = L"Regresar";
			   this->button4->UseVisualStyleBackColor = false;
			   this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			   // 
			   // button3
			   // 
			   this->button3->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button3->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button3->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->button3->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button3->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button3->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->button3->Location = System::Drawing::Point(81, 598);
			   this->button3->Margin = System::Windows::Forms::Padding(2);
			   this->button3->Name = L"button3";
			   this->button3->Padding = System::Windows::Forms::Padding(10, 5, 10, 5);
			   this->button3->Size = System::Drawing::Size(340, 42);
			   this->button3->TabIndex = 13;
			   this->button3->Text = L"Registrarse";
			   this->button3->UseVisualStyleBackColor = false;
			   this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
			   // 
			   // label11
			   // 
			   this->label11->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label11->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label11->ForeColor = System::Drawing::Color::White;
			   this->label11->Location = System::Drawing::Point(588, 440);
			   this->label11->Name = L"label11";
			   this->label11->Size = System::Drawing::Size(270, 33);
			   this->label11->TabIndex = 11;
			   this->label11->Text = L"Genero:                                                          ";
			   // 
			   // label12
			   // 
			   this->label12->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label12->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label12->ForeColor = System::Drawing::Color::White;
			   this->label12->Location = System::Drawing::Point(588, 373);
			   this->label12->Name = L"label12";
			   this->label12->Size = System::Drawing::Size(270, 31);
			   this->label12->TabIndex = 10;
			   this->label12->Text = L"Pais:                                                               ";
			   // 
			   // label13
			   // 
			   this->label13->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label13->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label13->ForeColor = System::Drawing::Color::White;
			   this->label13->Location = System::Drawing::Point(588, 312);
			   this->label13->Name = L"label13";
			   this->label13->Size = System::Drawing::Size(270, 24);
			   this->label13->TabIndex = 9;
			   this->label13->Text = L"Fecha De Nacimiento:                              ";
			   // 
			   // label9
			   // 
			   this->label9->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label9->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label9->ForeColor = System::Drawing::Color::White;
			   this->label9->Location = System::Drawing::Point(587, 185);
			   this->label9->Name = L"label9";
			   this->label9->Size = System::Drawing::Size(270, 20);
			   this->label9->TabIndex = 6;
			   this->label9->Text = L"Contraseña:                                                   ";
			   // 
			   // label8
			   // 
			   this->label8->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label8->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label8->ForeColor = System::Drawing::Color::White;
			   this->label8->Location = System::Drawing::Point(149, 374);
			   this->label8->Name = L"label8";
			   this->label8->Size = System::Drawing::Size(270, 22);
			   this->label8->TabIndex = 5;
			   this->label8->Text = L"Nombre:                                                       ";
			   // 
			   // label7
			   // 
			   this->label7->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label7->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label7->ForeColor = System::Drawing::Color::White;
			   this->label7->Location = System::Drawing::Point(150, 310);
			   this->label7->Name = L"label7";
			   this->label7->Size = System::Drawing::Size(270, 25);
			   this->label7->TabIndex = 4;
			   this->label7->Text = L"Nombre Artistico:                                         ";
			   // 
			   // label5
			   // 
			   this->label5->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label5->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label5->ForeColor = System::Drawing::Color::White;
			   this->label5->Location = System::Drawing::Point(149, 251);
			   this->label5->Name = L"label5";
			   this->label5->Size = System::Drawing::Size(270, 19);
			   this->label5->TabIndex = 2;
			   this->label5->Text = L"Correo Electronico:                                        ";
			   // 
			   // label4
			   // 
			   this->label4->AutoSize = true;
			   this->label4->Font = (gcnew System::Drawing::Font(L"Segoe UI", 28.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label4->ForeColor = System::Drawing::Color::White;
			   this->label4->Location = System::Drawing::Point(538, 59);
			   this->label4->Name = L"label4";
			   this->label4->Size = System::Drawing::Size(358, 51);
			   this->label4->TabIndex = 1;
			   this->label4->Text = L"Registro de Artista";
			   // 
			   // panel18
			   // 
			   this->panel18->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel18->Controls->Add(this->label10);
			   this->panel18->Controls->Add(this->button5);
			   this->panel18->Controls->Add(this->pictureBox2);
			   this->panel18->Location = System::Drawing::Point(914, 184);
			   this->panel18->Name = L"panel18";
			   this->panel18->Size = System::Drawing::Size(407, 456);
			   this->panel18->TabIndex = 61;
			   // 
			   // label10
			   // 
			   this->label10->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label10->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label10->ForeColor = System::Drawing::Color::White;
			   this->label10->Location = System::Drawing::Point(36, 22);
			   this->label10->Name = L"label10";
			   this->label10->Size = System::Drawing::Size(338, 19);
			   this->label10->TabIndex = 7;
			   this->label10->Text = L"Imagen:                                                         ";
			   // 
			   // button5
			   // 
			   this->button5->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button5->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button5->FlatAppearance->BorderSize = 0;
			   this->button5->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button5->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button5->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->button5->Location = System::Drawing::Point(135, 353);
			   this->button5->Margin = System::Windows::Forms::Padding(2);
			   this->button5->Name = L"button5";
			   this->button5->Size = System::Drawing::Size(158, 35);
			   this->button5->TabIndex = 20;
			   this->button5->Text = L"Seleccionar Imagen";
			   this->button5->UseVisualStyleBackColor = false;
			   this->button5->Click += gcnew System::EventHandler(this, &MyForm::button5_Click);
			   // 
			   // pictureBox2
			   // 
			   this->pictureBox2->BackColor = System::Drawing::Color::Transparent;
			   this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox2.Image")));
			   this->pictureBox2->Location = System::Drawing::Point(95, 100);
			   this->pictureBox2->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox2->Name = L"pictureBox2";
			   this->pictureBox2->Size = System::Drawing::Size(222, 186);
			   this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox2->TabIndex = 19;
			   this->pictureBox2->TabStop = false;
			   // 
			   // PanelRegistroUsuario
			   // 
			   this->PanelRegistroUsuario->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(12)),
				   static_cast<System::Int32>(static_cast<System::Byte>(12)), static_cast<System::Int32>(static_cast<System::Byte>(12)));
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox21);
			   this->PanelRegistroUsuario->Controls->Add(this->idboxusu);
			   this->PanelRegistroUsuario->Controls->Add(this->label32);
			   this->PanelRegistroUsuario->Controls->Add(this->correou);
			   this->PanelRegistroUsuario->Controls->Add(this->label31);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox24);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox25);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox28);
			   this->PanelRegistroUsuario->Controls->Add(this->panel5);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox20);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox22);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox23);
			   this->PanelRegistroUsuario->Controls->Add(this->pictureBox19);
			   this->PanelRegistroUsuario->Controls->Add(this->edad2);
			   this->PanelRegistroUsuario->Controls->Add(this->valicontra2);
			   this->PanelRegistroUsuario->Controls->Add(this->contra2);
			   this->PanelRegistroUsuario->Controls->Add(this->Nombre2);
			   this->PanelRegistroUsuario->Controls->Add(this->Usuario2);
			   this->PanelRegistroUsuario->Controls->Add(this->button7);
			   this->PanelRegistroUsuario->Controls->Add(this->button8);
			   this->PanelRegistroUsuario->Controls->Add(this->label17);
			   this->PanelRegistroUsuario->Controls->Add(this->label18);
			   this->PanelRegistroUsuario->Controls->Add(this->label20);
			   this->PanelRegistroUsuario->Controls->Add(this->label21);
			   this->PanelRegistroUsuario->Controls->Add(this->label23);
			   this->PanelRegistroUsuario->Controls->Add(this->label25);
			   this->PanelRegistroUsuario->Controls->Add(this->panel19);
			   this->PanelRegistroUsuario->Location = System::Drawing::Point(0, 0);
			   this->PanelRegistroUsuario->Margin = System::Windows::Forms::Padding(2);
			   this->PanelRegistroUsuario->Name = L"PanelRegistroUsuario";
			   this->PanelRegistroUsuario->Size = System::Drawing::Size(1396, 734);
			   this->PanelRegistroUsuario->TabIndex = 10;
			   // 
			   // pictureBox21
			   // 
			   this->pictureBox21->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox21->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox21.Image")));
			   this->pictureBox21->Location = System::Drawing::Point(518, 389);
			   this->pictureBox21->Name = L"pictureBox21";
			   this->pictureBox21->Size = System::Drawing::Size(63, 47);
			   this->pictureBox21->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox21->TabIndex = 57;
			   this->pictureBox21->TabStop = false;
			   // 
			   // idboxusu
			   // 
			   this->idboxusu->BackColor = System::Drawing::SystemColors::ButtonFace;
			   this->idboxusu->Enabled = false;
			   this->idboxusu->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->idboxusu->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->idboxusu->Location = System::Drawing::Point(600, 410);
			   this->idboxusu->Margin = System::Windows::Forms::Padding(2);
			   this->idboxusu->Name = L"idboxusu";
			   this->idboxusu->Size = System::Drawing::Size(271, 26);
			   this->idboxusu->TabIndex = 56;
			   // 
			   // label32
			   // 
			   this->label32->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label32->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label32->ForeColor = System::Drawing::Color::White;
			   this->label32->Location = System::Drawing::Point(600, 391);
			   this->label32->Name = L"label32";
			   this->label32->Size = System::Drawing::Size(271, 19);
			   this->label32->TabIndex = 55;
			   this->label32->Text = L"Id: ";
			   // 
			   // correou
			   // 
			   this->correou->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->correou->Location = System::Drawing::Point(157, 350);
			   this->correou->Margin = System::Windows::Forms::Padding(2);
			   this->correou->Name = L"correou";
			   this->correou->Size = System::Drawing::Size(270, 26);
			   this->correou->TabIndex = 54;
			   // 
			   // label31
			   // 
			   this->label31->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label31->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label31->ForeColor = System::Drawing::Color::White;
			   this->label31->Location = System::Drawing::Point(157, 331);
			   this->label31->Name = L"label31";
			   this->label31->Size = System::Drawing::Size(270, 19);
			   this->label31->TabIndex = 53;
			   this->label31->Text = L"Correo Electronico:                                                       ";
			   // 
			   // pictureBox24
			   // 
			   this->pictureBox24->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox24->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox24.Image")));
			   this->pictureBox24->Location = System::Drawing::Point(518, 271);
			   this->pictureBox24->Name = L"pictureBox24";
			   this->pictureBox24->Size = System::Drawing::Size(63, 48);
			   this->pictureBox24->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox24->TabIndex = 52;
			   this->pictureBox24->TabStop = false;
			   // 
			   // pictureBox25
			   // 
			   this->pictureBox25->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox25->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox25.Image")));
			   this->pictureBox25->Location = System::Drawing::Point(518, 213);
			   this->pictureBox25->Name = L"pictureBox25";
			   this->pictureBox25->Size = System::Drawing::Size(63, 48);
			   this->pictureBox25->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox25->TabIndex = 49;
			   this->pictureBox25->TabStop = false;
			   // 
			   // pictureBox28
			   // 
			   this->pictureBox28->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox28->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox28.Image")));
			   this->pictureBox28->Location = System::Drawing::Point(517, 328);
			   this->pictureBox28->Name = L"pictureBox28";
			   this->pictureBox28->Size = System::Drawing::Size(63, 47);
			   this->pictureBox28->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox28->TabIndex = 51;
			   this->pictureBox28->TabStop = false;
			   // 
			   // panel5
			   // 
			   this->panel5->BackColor = System::Drawing::Color::LightGray;
			   this->panel5->Location = System::Drawing::Point(465, 213);
			   this->panel5->Name = L"panel5";
			   this->panel5->Size = System::Drawing::Size(20, 340);
			   this->panel5->TabIndex = 47;
			   // 
			   // pictureBox20
			   // 
			   this->pictureBox20->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox20->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox20.Image")));
			   this->pictureBox20->Location = System::Drawing::Point(74, 272);
			   this->pictureBox20->Name = L"pictureBox20";
			   this->pictureBox20->Size = System::Drawing::Size(63, 48);
			   this->pictureBox20->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox20->TabIndex = 42;
			   this->pictureBox20->TabStop = false;
			   // 
			   // pictureBox22
			   // 
			   this->pictureBox22->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox22->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			   this->pictureBox22->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox22.Image")));
			   this->pictureBox22->Location = System::Drawing::Point(75, 329);
			   this->pictureBox22->Name = L"pictureBox22";
			   this->pictureBox22->Size = System::Drawing::Size(63, 48);
			   this->pictureBox22->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox22->TabIndex = 40;
			   this->pictureBox22->TabStop = false;
			   this->pictureBox22->UseWaitCursor = true;
			   // 
			   // pictureBox23
			   // 
			   this->pictureBox23->BackColor = System::Drawing::Color::LightGray;
			   this->pictureBox23->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox23.Image")));
			   this->pictureBox23->Location = System::Drawing::Point(73, 214);
			   this->pictureBox23->Name = L"pictureBox23";
			   this->pictureBox23->Size = System::Drawing::Size(64, 49);
			   this->pictureBox23->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox23->TabIndex = 39;
			   this->pictureBox23->TabStop = false;
			   // 
			   // pictureBox19
			   // 
			   this->pictureBox19->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox19.Image")));
			   this->pictureBox19->Location = System::Drawing::Point(466, 97);
			   this->pictureBox19->Name = L"pictureBox19";
			   this->pictureBox19->Size = System::Drawing::Size(63, 47);
			   this->pictureBox19->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox19->TabIndex = 37;
			   this->pictureBox19->TabStop = false;
			   // 
			   // edad2
			   // 
			   this->edad2->Enabled = false;
			   this->edad2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->edad2->Location = System::Drawing::Point(600, 353);
			   this->edad2->Margin = System::Windows::Forms::Padding(2);
			   this->edad2->Name = L"edad2";
			   this->edad2->Size = System::Drawing::Size(272, 23);
			   this->edad2->TabIndex = 26;
			   // 
			   // valicontra2
			   // 
			   this->valicontra2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->valicontra2->Location = System::Drawing::Point(600, 293);
			   this->valicontra2->Margin = System::Windows::Forms::Padding(2);
			   this->valicontra2->Name = L"valicontra2";
			   this->valicontra2->PasswordChar = '*';
			   this->valicontra2->Size = System::Drawing::Size(271, 26);
			   this->valicontra2->TabIndex = 22;
			   this->valicontra2->UseSystemPasswordChar = true;
			   // 
			   // contra2
			   // 
			   this->contra2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->contra2->Location = System::Drawing::Point(600, 232);
			   this->contra2->Margin = System::Windows::Forms::Padding(2);
			   this->contra2->Name = L"contra2";
			   this->contra2->PasswordChar = '*';
			   this->contra2->Size = System::Drawing::Size(271, 26);
			   this->contra2->TabIndex = 21;
			   this->contra2->UseSystemPasswordChar = true;
			   // 
			   // Nombre2
			   // 
			   this->Nombre2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->Nombre2->Location = System::Drawing::Point(157, 295);
			   this->Nombre2->Margin = System::Windows::Forms::Padding(2);
			   this->Nombre2->Multiline = true;
			   this->Nombre2->Name = L"Nombre2";
			   this->Nombre2->Size = System::Drawing::Size(270, 25);
			   this->Nombre2->TabIndex = 18;
			   // 
			   // Usuario2
			   // 
			   this->Usuario2->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->Usuario2->Location = System::Drawing::Point(158, 232);
			   this->Usuario2->Margin = System::Windows::Forms::Padding(2);
			   this->Usuario2->Name = L"Usuario2";
			   this->Usuario2->Size = System::Drawing::Size(269, 26);
			   this->Usuario2->TabIndex = 15;
			   // 
			   // button7
			   // 
			   this->button7->BackColor = System::Drawing::Color::LightGray;
			   this->button7->FlatAppearance->BorderSize = 0;
			   this->button7->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button7->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button7->ForeColor = System::Drawing::Color::Black;
			   this->button7->Location = System::Drawing::Point(517, 510);
			   this->button7->Margin = System::Windows::Forms::Padding(2);
			   this->button7->Name = L"button7";
			   this->button7->Size = System::Drawing::Size(357, 42);
			   this->button7->TabIndex = 14;
			   this->button7->Text = L"Regresar";
			   this->button7->UseVisualStyleBackColor = false;
			   this->button7->Click += gcnew System::EventHandler(this, &MyForm::button7_Click);
			   // 
			   // button8
			   // 
			   this->button8->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button8->FlatAppearance->BorderSize = 0;
			   this->button8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button8->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button8->ForeColor = System::Drawing::Color::White;
			   this->button8->Location = System::Drawing::Point(74, 510);
			   this->button8->Margin = System::Windows::Forms::Padding(2);
			   this->button8->Name = L"button8";
			   this->button8->Size = System::Drawing::Size(353, 42);
			   this->button8->TabIndex = 13;
			   this->button8->Text = L"Registrarse";
			   this->button8->UseVisualStyleBackColor = false;
			   this->button8->Click += gcnew System::EventHandler(this, &MyForm::button8_Click);
			   // 
			   // label17
			   // 
			   this->label17->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label17->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label17->ForeColor = System::Drawing::Color::White;
			   this->label17->Location = System::Drawing::Point(600, 328);
			   this->label17->Name = L"label17";
			   this->label17->Size = System::Drawing::Size(272, 25);
			   this->label17->TabIndex = 9;
			   this->label17->Text = L"Fecha de Creacion:";
			   // 
			   // label18
			   // 
			   this->label18->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label18->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label18->ForeColor = System::Drawing::Color::White;
			   this->label18->Location = System::Drawing::Point(600, 271);
			   this->label18->Name = L"label18";
			   this->label18->Size = System::Drawing::Size(271, 25);
			   this->label18->TabIndex = 8;
			   this->label18->Text = L"Confirmar:                                                    ";
			   // 
			   // label20
			   // 
			   this->label20->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label20->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label20->ForeColor = System::Drawing::Color::White;
			   this->label20->Location = System::Drawing::Point(600, 213);
			   this->label20->Name = L"label20";
			   this->label20->Size = System::Drawing::Size(271, 19);
			   this->label20->TabIndex = 6;
			   this->label20->Text = L"Contraseña:                                                   ";
			   // 
			   // label21
			   // 
			   this->label21->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label21->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label21->ForeColor = System::Drawing::Color::White;
			   this->label21->Location = System::Drawing::Point(157, 272);
			   this->label21->Name = L"label21";
			   this->label21->Size = System::Drawing::Size(270, 23);
			   this->label21->TabIndex = 5;
			   this->label21->Text = L"Nombre:                                                       ";
			   // 
			   // label23
			   // 
			   this->label23->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label23->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label23->ForeColor = System::Drawing::Color::White;
			   this->label23->Location = System::Drawing::Point(157, 214);
			   this->label23->Name = L"label23";
			   this->label23->Size = System::Drawing::Size(270, 19);
			   this->label23->TabIndex = 3;
			   this->label23->Text = L"Usuario:                                                         ";
			   // 
			   // label25
			   // 
			   this->label25->AutoSize = true;
			   this->label25->Font = (gcnew System::Drawing::Font(L"Segoe UI", 28.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label25->ForeColor = System::Drawing::Color::White;
			   this->label25->Location = System::Drawing::Point(540, 90);
			   this->label25->Name = L"label25";
			   this->label25->Size = System::Drawing::Size(373, 51);
			   this->label25->TabIndex = 1;
			   this->label25->Text = L"Registro de Usuario";
			   // 
			   // panel19
			   // 
			   this->panel19->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel19->Controls->Add(this->pictureBox3);
			   this->panel19->Controls->Add(this->button6);
			   this->panel19->Controls->Add(this->label19);
			   this->panel19->Location = System::Drawing::Point(929, 213);
			   this->panel19->Name = L"panel19";
			   this->panel19->Size = System::Drawing::Size(389, 344);
			   this->panel19->TabIndex = 58;
			   // 
			   // pictureBox3
			   // 
			   this->pictureBox3->BackColor = System::Drawing::Color::Transparent;
			   this->pictureBox3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox3.Image")));
			   this->pictureBox3->Location = System::Drawing::Point(72, 61);
			   this->pictureBox3->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox3->Name = L"pictureBox3";
			   this->pictureBox3->Size = System::Drawing::Size(257, 186);
			   this->pictureBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox3->TabIndex = 19;
			   this->pictureBox3->TabStop = false;
			   // 
			   // button6
			   // 
			   this->button6->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button6->FlatAppearance->BorderSize = 0;
			   this->button6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button6->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button6->ForeColor = System::Drawing::Color::White;
			   this->button6->Location = System::Drawing::Point(108, 266);
			   this->button6->Margin = System::Windows::Forms::Padding(2);
			   this->button6->Name = L"button6";
			   this->button6->Size = System::Drawing::Size(176, 38);
			   this->button6->TabIndex = 20;
			   this->button6->Text = L"Seleccionar Imagen";
			   this->button6->UseVisualStyleBackColor = false;
			   this->button6->Click += gcnew System::EventHandler(this, &MyForm::button6_Click);
			   // 
			   // label19
			   // 
			   this->label19->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label19->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label19->ForeColor = System::Drawing::Color::White;
			   this->label19->Location = System::Drawing::Point(14, 21);
			   this->label19->Name = L"label19";
			   this->label19->Size = System::Drawing::Size(352, 27);
			   this->label19->TabIndex = 7;
			   this->label19->Text = L"Imagen:                                                         ";
			   // 
			   // PanelUsuario
			   // 
			   this->PanelUsuario->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(12)), static_cast<System::Int32>(static_cast<System::Byte>(12)),
				   static_cast<System::Int32>(static_cast<System::Byte>(12)));
			   this->PanelUsuario->Controls->Add(this->PanelCatalogo);
			   this->PanelUsuario->Controls->Add(this->Panelstatsusu);
			   this->PanelUsuario->Controls->Add(this->PanelConfig);
			   this->PanelUsuario->Controls->Add(this->PanelCrearPlay);
			   this->PanelUsuario->Controls->Add(this->PanelFav);
			   this->PanelUsuario->Controls->Add(this->PanelVerPlay);
			   this->PanelUsuario->Controls->Add(this->panel14);
			   this->PanelUsuario->Controls->Add(this->panel11);
			   this->PanelUsuario->Controls->Add(this->label22);
			   this->PanelUsuario->Controls->Add(this->pictureBox7);
			   this->PanelUsuario->Controls->Add(this->label24);
			   this->PanelUsuario->Location = System::Drawing::Point(0, 0);
			   this->PanelUsuario->Margin = System::Windows::Forms::Padding(2);
			   this->PanelUsuario->Name = L"PanelUsuario";
			   this->PanelUsuario->Size = System::Drawing::Size(835, 598);
			   this->PanelUsuario->TabIndex = 16;
			   // 
			   // PanelCatalogo
			   // 
			   this->PanelCatalogo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelCatalogo->Controls->Add(this->pictureBox33);
			   this->PanelCatalogo->Controls->Add(this->textBox3);
			   this->PanelCatalogo->Controls->Add(this->listBox1);
			   this->PanelCatalogo->ForeColor = System::Drawing::Color::White;
			   this->PanelCatalogo->Location = System::Drawing::Point(236, 91);
			   this->PanelCatalogo->Margin = System::Windows::Forms::Padding(2);
			   this->PanelCatalogo->Name = L"PanelCatalogo";
			   this->PanelCatalogo->Size = System::Drawing::Size(579, 425);
			   this->PanelCatalogo->TabIndex = 18;
			   // 
			   // pictureBox33
			   // 
			   this->pictureBox33->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox33->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox33.Image")));
			   this->pictureBox33->Location = System::Drawing::Point(8, 22);
			   this->pictureBox33->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox33->Name = L"pictureBox33";
			   this->pictureBox33->Size = System::Drawing::Size(34, 28);
			   this->pictureBox33->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox33->TabIndex = 28;
			   this->pictureBox33->TabStop = false;
			   this->pictureBox33->MouseEnter += gcnew System::EventHandler(this, &MyForm::pictureBox33_MouseEnter);
			   this->pictureBox33->MouseLeave += gcnew System::EventHandler(this, &MyForm::pictureBox33_MouseLeave);
			   // 
			   // textBox3
			   // 
			   this->textBox3->BackColor = System::Drawing::Color::DimGray;
			   this->textBox3->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox3->Location = System::Drawing::Point(46, 22);
			   this->textBox3->Margin = System::Windows::Forms::Padding(2);
			   this->textBox3->Multiline = true;
			   this->textBox3->Name = L"textBox3";
			   this->textBox3->Size = System::Drawing::Size(522, 28);
			   this->textBox3->TabIndex = 1;
			   // 
			   // listBox1
			   // 
			   this->listBox1->FormattingEnabled = true;
			   this->listBox1->Location = System::Drawing::Point(10, 66);
			   this->listBox1->Margin = System::Windows::Forms::Padding(2);
			   this->listBox1->Name = L"listBox1";
			   this->listBox1->Size = System::Drawing::Size(559, 342);
			   this->listBox1->TabIndex = 0;
			   // 
			   // Panelstatsusu
			   // 
			   this->Panelstatsusu->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->Panelstatsusu->Controls->Add(this->dataGridView4);
			   this->Panelstatsusu->Controls->Add(this->label80);
			   this->Panelstatsusu->Controls->Add(this->label81);
			   this->Panelstatsusu->Controls->Add(this->label82);
			   this->Panelstatsusu->Controls->Add(this->label83);
			   this->Panelstatsusu->Controls->Add(this->listBox15);
			   this->Panelstatsusu->Controls->Add(this->listBox16);
			   this->Panelstatsusu->Controls->Add(this->listBox18);
			   this->Panelstatsusu->ForeColor = System::Drawing::Color::White;
			   this->Panelstatsusu->Location = System::Drawing::Point(236, 86);
			   this->Panelstatsusu->Margin = System::Windows::Forms::Padding(2);
			   this->Panelstatsusu->Name = L"Panelstatsusu";
			   this->Panelstatsusu->Size = System::Drawing::Size(580, 431);
			   this->Panelstatsusu->TabIndex = 81;
			   // 
			   // dataGridView4
			   // 
			   this->dataGridView4->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			   this->dataGridView4->Location = System::Drawing::Point(16, 230);
			   this->dataGridView4->Margin = System::Windows::Forms::Padding(2);
			   this->dataGridView4->Name = L"dataGridView4";
			   this->dataGridView4->RowHeadersWidth = 51;
			   this->dataGridView4->RowTemplate->Height = 24;
			   this->dataGridView4->Size = System::Drawing::Size(546, 181);
			   this->dataGridView4->TabIndex = 72;
			   // 
			   // label80
			   // 
			   this->label80->AutoSize = true;
			   this->label80->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label80->ForeColor = System::Drawing::Color::White;
			   this->label80->Location = System::Drawing::Point(194, 206);
			   this->label80->Name = L"label80";
			   this->label80->Size = System::Drawing::Size(202, 13);
			   this->label80->TabIndex = 71;
			   this->label80->Text = L"Promedio de calificaciones Otrogadas";
			   this->label80->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label81
			   // 
			   this->label81->AutoSize = true;
			   this->label81->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label81->ForeColor = System::Drawing::Color::White;
			   this->label81->Location = System::Drawing::Point(413, 21);
			   this->label81->Name = L"label81";
			   this->label81->Size = System::Drawing::Size(146, 13);
			   this->label81->TabIndex = 70;
			   this->label81->Text = L"Canciones mas Escuchadas";
			   this->label81->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label82
			   // 
			   this->label82->AutoSize = true;
			   this->label82->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label82->ForeColor = System::Drawing::Color::White;
			   this->label82->Location = System::Drawing::Point(216, 21);
			   this->label82->Name = L"label82";
			   this->label82->Size = System::Drawing::Size(137, 13);
			   this->label82->TabIndex = 69;
			   this->label82->Text = L"Tiempo de Reproduccion";
			   this->label82->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label83
			   // 
			   this->label83->AutoSize = true;
			   this->label83->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label83->ForeColor = System::Drawing::Color::White;
			   this->label83->Location = System::Drawing::Point(16, 20);
			   this->label83->Name = L"label83";
			   this->label83->Size = System::Drawing::Size(212, 13);
			   this->label83->TabIndex = 68;
			   this->label83->Text = L" Numero total de canciones Escuchadas";
			   this->label83->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // listBox15
			   // 
			   this->listBox15->FormattingEnabled = true;
			   this->listBox15->Location = System::Drawing::Point(217, 43);
			   this->listBox15->Margin = System::Windows::Forms::Padding(2);
			   this->listBox15->Name = L"listBox15";
			   this->listBox15->Size = System::Drawing::Size(148, 147);
			   this->listBox15->TabIndex = 67;
			   // 
			   // listBox16
			   // 
			   this->listBox16->FormattingEnabled = true;
			   this->listBox16->Location = System::Drawing::Point(416, 46);
			   this->listBox16->Margin = System::Windows::Forms::Padding(2);
			   this->listBox16->Name = L"listBox16";
			   this->listBox16->Size = System::Drawing::Size(148, 147);
			   this->listBox16->TabIndex = 66;
			   // 
			   // listBox18
			   // 
			   this->listBox18->FormattingEnabled = true;
			   this->listBox18->Location = System::Drawing::Point(18, 43);
			   this->listBox18->Margin = System::Windows::Forms::Padding(2);
			   this->listBox18->Name = L"listBox18";
			   this->listBox18->Size = System::Drawing::Size(148, 147);
			   this->listBox18->TabIndex = 65;
			   // 
			   // PanelConfig
			   // 
			   this->PanelConfig->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelConfig->Controls->Add(this->button12);
			   this->PanelConfig->Controls->Add(this->label39);
			   this->PanelConfig->Controls->Add(this->pictureBox44);
			   this->PanelConfig->Controls->Add(this->textBox9);
			   this->PanelConfig->Controls->Add(this->label40);
			   this->PanelConfig->Controls->Add(this->textBox10);
			   this->PanelConfig->Controls->Add(this->textBox11);
			   this->PanelConfig->Controls->Add(this->button15);
			   this->PanelConfig->Controls->Add(this->textBox12);
			   this->PanelConfig->Controls->Add(this->textBox13);
			   this->PanelConfig->Controls->Add(this->button17);
			   this->PanelConfig->Controls->Add(this->label42);
			   this->PanelConfig->Controls->Add(this->label43);
			   this->PanelConfig->Controls->Add(this->label44);
			   this->PanelConfig->Controls->Add(this->label45);
			   this->PanelConfig->Controls->Add(this->label46);
			   this->PanelConfig->ForeColor = System::Drawing::Color::White;
			   this->PanelConfig->Location = System::Drawing::Point(236, 87);
			   this->PanelConfig->Margin = System::Windows::Forms::Padding(2);
			   this->PanelConfig->Name = L"PanelConfig";
			   this->PanelConfig->Size = System::Drawing::Size(579, 431);
			   this->PanelConfig->TabIndex = 34;
			   // 
			   // button12
			   // 
			   this->button12->BackColor = System::Drawing::Color::LightGray;
			   this->button12->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button12->FlatAppearance->BorderSize = 0;
			   this->button12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button12->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button12->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->button12->Location = System::Drawing::Point(295, 352);
			   this->button12->Margin = System::Windows::Forms::Padding(2);
			   this->button12->Name = L"button12";
			   this->button12->Padding = System::Windows::Forms::Padding(10, 5, 10, 5);
			   this->button12->Size = System::Drawing::Size(271, 42);
			   this->button12->TabIndex = 80;
			   this->button12->Text = L"Elimnar Cuenta";
			   this->button12->UseVisualStyleBackColor = false;
			   // 
			   // label39
			   // 
			   this->label39->AutoSize = true;
			   this->label39->Font = (gcnew System::Drawing::Font(L"Segoe UI", 19.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label39->ForeColor = System::Drawing::Color::White;
			   this->label39->Location = System::Drawing::Point(157, 24);
			   this->label39->Name = L"label39";
			   this->label39->Size = System::Drawing::Size(261, 37);
			   this->label39->TabIndex = 35;
			   this->label39->Text = L"Configurar Usuario";
			   // 
			   // pictureBox44
			   // 
			   this->pictureBox44->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox44.Image")));
			   this->pictureBox44->Location = System::Drawing::Point(448, 257);
			   this->pictureBox44->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox44->Name = L"pictureBox44";
			   this->pictureBox44->Size = System::Drawing::Size(118, 70);
			   this->pictureBox44->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox44->TabIndex = 30;
			   this->pictureBox44->TabStop = false;
			   // 
			   // textBox9
			   // 
			   this->textBox9->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox9->Location = System::Drawing::Point(13, 224);
			   this->textBox9->Margin = System::Windows::Forms::Padding(2);
			   this->textBox9->Name = L"textBox9";
			   this->textBox9->Size = System::Drawing::Size(271, 26);
			   this->textBox9->TabIndex = 77;
			   // 
			   // label40
			   // 
			   this->label40->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label40->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label40->ForeColor = System::Drawing::Color::White;
			   this->label40->Location = System::Drawing::Point(14, 206);
			   this->label40->Name = L"label40";
			   this->label40->Size = System::Drawing::Size(270, 19);
			   this->label40->TabIndex = 76;
			   this->label40->Text = L"Correo Electronico:                                                       ";
			   // 
			   // textBox10
			   // 
			   this->textBox10->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox10->Location = System::Drawing::Point(295, 170);
			   this->textBox10->Margin = System::Windows::Forms::Padding(2);
			   this->textBox10->Name = L"textBox10";
			   this->textBox10->PasswordChar = '*';
			   this->textBox10->Size = System::Drawing::Size(272, 26);
			   this->textBox10->TabIndex = 70;
			   this->textBox10->UseSystemPasswordChar = true;
			   // 
			   // textBox11
			   // 
			   this->textBox11->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox11->Location = System::Drawing::Point(295, 109);
			   this->textBox11->Margin = System::Windows::Forms::Padding(2);
			   this->textBox11->Name = L"textBox11";
			   this->textBox11->PasswordChar = '*';
			   this->textBox11->Size = System::Drawing::Size(272, 26);
			   this->textBox11->TabIndex = 69;
			   this->textBox11->UseSystemPasswordChar = true;
			   // 
			   // button15
			   // 
			   this->button15->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button15->FlatAppearance->BorderSize = 0;
			   this->button15->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button15->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button15->ForeColor = System::Drawing::Color::White;
			   this->button15->Location = System::Drawing::Point(295, 257);
			   this->button15->Margin = System::Windows::Forms::Padding(2);
			   this->button15->Name = L"button15";
			   this->button15->Size = System::Drawing::Size(118, 70);
			   this->button15->TabIndex = 68;
			   this->button15->Text = L"Seleccionar Imagen";
			   this->button15->UseVisualStyleBackColor = false;
			   // 
			   // textBox12
			   // 
			   this->textBox12->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox12->Location = System::Drawing::Point(14, 169);
			   this->textBox12->Margin = System::Windows::Forms::Padding(2);
			   this->textBox12->Multiline = true;
			   this->textBox12->Name = L"textBox12";
			   this->textBox12->Size = System::Drawing::Size(271, 25);
			   this->textBox12->TabIndex = 67;
			   // 
			   // textBox13
			   // 
			   this->textBox13->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox13->Location = System::Drawing::Point(14, 106);
			   this->textBox13->Margin = System::Windows::Forms::Padding(2);
			   this->textBox13->Name = L"textBox13";
			   this->textBox13->Size = System::Drawing::Size(270, 26);
			   this->textBox13->TabIndex = 66;
			   // 
			   // button17
			   // 
			   this->button17->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button17->FlatAppearance->BorderSize = 0;
			   this->button17->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button17->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button17->ForeColor = System::Drawing::Color::White;
			   this->button17->Location = System::Drawing::Point(10, 352);
			   this->button17->Margin = System::Windows::Forms::Padding(2);
			   this->button17->Name = L"button17";
			   this->button17->Size = System::Drawing::Size(271, 42);
			   this->button17->TabIndex = 64;
			   this->button17->Text = L"Editar";
			   this->button17->UseVisualStyleBackColor = false;
			   // 
			   // label42
			   // 
			   this->label42->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label42->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label42->ForeColor = System::Drawing::Color::White;
			   this->label42->Location = System::Drawing::Point(295, 148);
			   this->label42->Name = L"label42";
			   this->label42->Size = System::Drawing::Size(271, 25);
			   this->label42->TabIndex = 62;
			   this->label42->Text = L"Confirmar:                                                    ";
			   // 
			   // label43
			   // 
			   this->label43->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label43->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label43->ForeColor = System::Drawing::Color::White;
			   this->label43->Location = System::Drawing::Point(296, 206);
			   this->label43->Name = L"label43";
			   this->label43->Size = System::Drawing::Size(270, 19);
			   this->label43->TabIndex = 61;
			   this->label43->Text = L"Imagen:                                                         ";
			   // 
			   // label44
			   // 
			   this->label44->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label44->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label44->ForeColor = System::Drawing::Color::White;
			   this->label44->Location = System::Drawing::Point(295, 90);
			   this->label44->Name = L"label44";
			   this->label44->Size = System::Drawing::Size(271, 19);
			   this->label44->TabIndex = 60;
			   this->label44->Text = L"Contraseña:                                                   ";
			   // 
			   // label45
			   // 
			   this->label45->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label45->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label45->ForeColor = System::Drawing::Color::White;
			   this->label45->Location = System::Drawing::Point(14, 146);
			   this->label45->Name = L"label45";
			   this->label45->Size = System::Drawing::Size(270, 23);
			   this->label45->TabIndex = 59;
			   this->label45->Text = L"Nombre:                                                       ";
			   // 
			   // label46
			   // 
			   this->label46->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label46->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label46->ForeColor = System::Drawing::Color::White;
			   this->label46->Location = System::Drawing::Point(14, 89);
			   this->label46->Name = L"label46";
			   this->label46->Size = System::Drawing::Size(270, 19);
			   this->label46->TabIndex = 58;
			   this->label46->Text = L"Usuario:                                                         ";
			   // 
			   // PanelCrearPlay
			   // 
			   this->PanelCrearPlay->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelCrearPlay->Controls->Add(this->label37);
			   this->PanelCrearPlay->Controls->Add(this->label36);
			   this->PanelCrearPlay->Controls->Add(this->pictureBox41);
			   this->PanelCrearPlay->Controls->Add(this->label35);
			   this->PanelCrearPlay->Controls->Add(this->textBox6);
			   this->PanelCrearPlay->Controls->Add(this->listBox4);
			   this->PanelCrearPlay->Controls->Add(this->pictureBox40);
			   this->PanelCrearPlay->Controls->Add(this->textBox5);
			   this->PanelCrearPlay->ForeColor = System::Drawing::Color::White;
			   this->PanelCrearPlay->Location = System::Drawing::Point(236, 91);
			   this->PanelCrearPlay->Margin = System::Windows::Forms::Padding(2);
			   this->PanelCrearPlay->Name = L"PanelCrearPlay";
			   this->PanelCrearPlay->Size = System::Drawing::Size(579, 425);
			   this->PanelCrearPlay->TabIndex = 30;
			   // 
			   // label37
			   // 
			   this->label37->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label37->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label37->ForeColor = System::Drawing::Color::White;
			   this->label37->Location = System::Drawing::Point(51, 346);
			   this->label37->Name = L"label37";
			   this->label37->Size = System::Drawing::Size(142, 37);
			   this->label37->TabIndex = 28;
			   this->label37->Text = L"Aceptar";
			   this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label37->MouseEnter += gcnew System::EventHandler(this, &MyForm::label37_MouseEnter);
			   this->label37->MouseLeave += gcnew System::EventHandler(this, &MyForm::label37_MouseLeave);
			   // 
			   // label36
			   // 
			   this->label36->AutoSize = true;
			   this->label36->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label36->ForeColor = System::Drawing::Color::White;
			   this->label36->Location = System::Drawing::Point(30, 163);
			   this->label36->Name = L"label36";
			   this->label36->Size = System::Drawing::Size(207, 25);
			   this->label36->TabIndex = 32;
			   this->label36->Text = L"Portada de la Playlist: ";
			   // 
			   // pictureBox41
			   // 
			   this->pictureBox41->BackColor = System::Drawing::Color::Gainsboro;
			   this->pictureBox41->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox41.Image")));
			   this->pictureBox41->Location = System::Drawing::Point(56, 210);
			   this->pictureBox41->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox41->Name = L"pictureBox41";
			   this->pictureBox41->Size = System::Drawing::Size(133, 109);
			   this->pictureBox41->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox41->TabIndex = 28;
			   this->pictureBox41->TabStop = false;
			   // 
			   // label35
			   // 
			   this->label35->AutoSize = true;
			   this->label35->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label35->ForeColor = System::Drawing::Color::White;
			   this->label35->Location = System::Drawing::Point(29, 41);
			   this->label35->Name = L"label35";
			   this->label35->Size = System::Drawing::Size(210, 25);
			   this->label35->TabIndex = 31;
			   this->label35->Text = L"Nombre de la Playlist: ";
			   // 
			   // textBox6
			   // 
			   this->textBox6->BackColor = System::Drawing::Color::DimGray;
			   this->textBox6->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox6->Location = System::Drawing::Point(34, 85);
			   this->textBox6->Margin = System::Windows::Forms::Padding(2);
			   this->textBox6->Multiline = true;
			   this->textBox6->Name = L"textBox6";
			   this->textBox6->Size = System::Drawing::Size(180, 30);
			   this->textBox6->TabIndex = 30;
			   // 
			   // listBox4
			   // 
			   this->listBox4->FormattingEnabled = true;
			   this->listBox4->Location = System::Drawing::Point(283, 66);
			   this->listBox4->Margin = System::Windows::Forms::Padding(2);
			   this->listBox4->Name = L"listBox4";
			   this->listBox4->Size = System::Drawing::Size(287, 342);
			   this->listBox4->TabIndex = 29;
			   // 
			   // pictureBox40
			   // 
			   this->pictureBox40->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox40->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox40.Image")));
			   this->pictureBox40->Location = System::Drawing::Point(276, 22);
			   this->pictureBox40->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox40->Name = L"pictureBox40";
			   this->pictureBox40->Size = System::Drawing::Size(34, 28);
			   this->pictureBox40->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox40->TabIndex = 28;
			   this->pictureBox40->TabStop = false;
			   // 
			   // textBox5
			   // 
			   this->textBox5->BackColor = System::Drawing::Color::DimGray;
			   this->textBox5->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox5->Location = System::Drawing::Point(308, 22);
			   this->textBox5->Margin = System::Windows::Forms::Padding(2);
			   this->textBox5->Multiline = true;
			   this->textBox5->Name = L"textBox5";
			   this->textBox5->Size = System::Drawing::Size(261, 28);
			   this->textBox5->TabIndex = 1;
			   // 
			   // PanelFav
			   // 
			   this->PanelFav->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelFav->Controls->Add(this->listBox5);
			   this->PanelFav->Controls->Add(this->pictureBox43);
			   this->PanelFav->Controls->Add(this->textBox8);
			   this->PanelFav->ForeColor = System::Drawing::Color::White;
			   this->PanelFav->Location = System::Drawing::Point(236, 91);
			   this->PanelFav->Margin = System::Windows::Forms::Padding(2);
			   this->PanelFav->Name = L"PanelFav";
			   this->PanelFav->Size = System::Drawing::Size(579, 425);
			   this->PanelFav->TabIndex = 33;
			   // 
			   // listBox5
			   // 
			   this->listBox5->BackColor = System::Drawing::SystemColors::WindowFrame;
			   this->listBox5->FormattingEnabled = true;
			   this->listBox5->Location = System::Drawing::Point(10, 66);
			   this->listBox5->Margin = System::Windows::Forms::Padding(2);
			   this->listBox5->Name = L"listBox5";
			   this->listBox5->Size = System::Drawing::Size(559, 342);
			   this->listBox5->TabIndex = 29;
			   // 
			   // pictureBox43
			   // 
			   this->pictureBox43->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox43->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox43.Image")));
			   this->pictureBox43->Location = System::Drawing::Point(10, 22);
			   this->pictureBox43->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox43->Name = L"pictureBox43";
			   this->pictureBox43->Size = System::Drawing::Size(34, 28);
			   this->pictureBox43->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox43->TabIndex = 28;
			   this->pictureBox43->TabStop = false;
			   // 
			   // textBox8
			   // 
			   this->textBox8->BackColor = System::Drawing::Color::DimGray;
			   this->textBox8->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox8->Location = System::Drawing::Point(46, 22);
			   this->textBox8->Margin = System::Windows::Forms::Padding(2);
			   this->textBox8->Multiline = true;
			   this->textBox8->Name = L"textBox8";
			   this->textBox8->Size = System::Drawing::Size(522, 28);
			   this->textBox8->TabIndex = 1;
			   // 
			   // PanelVerPlay
			   // 
			   this->PanelVerPlay->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelVerPlay->Controls->Add(this->listBox3);
			   this->PanelVerPlay->Controls->Add(this->pictureBox39);
			   this->PanelVerPlay->Controls->Add(this->textBox4);
			   this->PanelVerPlay->Controls->Add(this->listBox2);
			   this->PanelVerPlay->ForeColor = System::Drawing::Color::White;
			   this->PanelVerPlay->Location = System::Drawing::Point(236, 91);
			   this->PanelVerPlay->Margin = System::Windows::Forms::Padding(2);
			   this->PanelVerPlay->Name = L"PanelVerPlay";
			   this->PanelVerPlay->Size = System::Drawing::Size(579, 425);
			   this->PanelVerPlay->TabIndex = 29;
			   // 
			   // listBox3
			   // 
			   this->listBox3->FormattingEnabled = true;
			   this->listBox3->Location = System::Drawing::Point(248, 66);
			   this->listBox3->Margin = System::Windows::Forms::Padding(2);
			   this->listBox3->Name = L"listBox3";
			   this->listBox3->Size = System::Drawing::Size(321, 342);
			   this->listBox3->TabIndex = 29;
			   // 
			   // pictureBox39
			   // 
			   this->pictureBox39->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox39->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox39.Image")));
			   this->pictureBox39->Location = System::Drawing::Point(8, 22);
			   this->pictureBox39->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox39->Name = L"pictureBox39";
			   this->pictureBox39->Size = System::Drawing::Size(34, 28);
			   this->pictureBox39->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox39->TabIndex = 28;
			   this->pictureBox39->TabStop = false;
			   // 
			   // textBox4
			   // 
			   this->textBox4->BackColor = System::Drawing::Color::DimGray;
			   this->textBox4->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox4->Location = System::Drawing::Point(46, 22);
			   this->textBox4->Margin = System::Windows::Forms::Padding(2);
			   this->textBox4->Multiline = true;
			   this->textBox4->Name = L"textBox4";
			   this->textBox4->Size = System::Drawing::Size(522, 28);
			   this->textBox4->TabIndex = 1;
			   // 
			   // listBox2
			   // 
			   this->listBox2->FormattingEnabled = true;
			   this->listBox2->Location = System::Drawing::Point(10, 66);
			   this->listBox2->Margin = System::Windows::Forms::Padding(2);
			   this->listBox2->Name = L"listBox2";
			   this->listBox2->Size = System::Drawing::Size(194, 342);
			   this->listBox2->TabIndex = 0;
			   // 
			   // panel14
			   // 
			   this->panel14->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel14->Controls->Add(this->axWindowsMediaPlayer2);
			   this->panel14->Controls->Add(this->pictureBox38);
			   this->panel14->Controls->Add(this->pictureBox37);
			   this->panel14->Controls->Add(this->pictureBox36);
			   this->panel14->Controls->Add(this->pictureBox35);
			   this->panel14->Controls->Add(this->pictureBox34);
			   this->panel14->Controls->Add(this->label33);
			   this->panel14->Controls->Add(this->pictureBox32);
			   this->panel14->Location = System::Drawing::Point(236, 531);
			   this->panel14->Margin = System::Windows::Forms::Padding(2);
			   this->panel14->Name = L"panel14";
			   this->panel14->Size = System::Drawing::Size(580, 56);
			   this->panel14->TabIndex = 19;
			   // 
			   // axWindowsMediaPlayer2
			   // 
			   this->axWindowsMediaPlayer2->Enabled = true;
			   this->axWindowsMediaPlayer2->Location = System::Drawing::Point(228, 27);
			   this->axWindowsMediaPlayer2->Margin = System::Windows::Forms::Padding(2);
			   this->axWindowsMediaPlayer2->Name = L"axWindowsMediaPlayer2";
			   this->axWindowsMediaPlayer2->OcxState = (cli::safe_cast<System::Windows::Forms::AxHost::State^>(resources->GetObject(L"axWindowsMediaPlayer2.OcxState")));
			   this->axWindowsMediaPlayer2->Size = System::Drawing::Size(12, 10);
			   this->axWindowsMediaPlayer2->TabIndex = 29;
			   // 
			   // pictureBox38
			   // 
			   this->pictureBox38->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox38->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox38.Image")));
			   this->pictureBox38->Location = System::Drawing::Point(496, 11);
			   this->pictureBox38->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox38->Name = L"pictureBox38";
			   this->pictureBox38->Size = System::Drawing::Size(40, 34);
			   this->pictureBox38->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox38->TabIndex = 34;
			   this->pictureBox38->TabStop = false;
			   // 
			   // pictureBox37
			   // 
			   this->pictureBox37->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox37->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox37.Image")));
			   this->pictureBox37->Location = System::Drawing::Point(438, 8);
			   this->pictureBox37->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox37->Name = L"pictureBox37";
			   this->pictureBox37->Size = System::Drawing::Size(47, 42);
			   this->pictureBox37->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox37->TabIndex = 33;
			   this->pictureBox37->TabStop = false;
			   // 
			   // pictureBox36
			   // 
			   this->pictureBox36->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox36->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox36.Image")));
			   this->pictureBox36->Location = System::Drawing::Point(370, 2);
			   this->pictureBox36->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox36->Name = L"pictureBox36";
			   this->pictureBox36->Size = System::Drawing::Size(64, 52);
			   this->pictureBox36->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox36->TabIndex = 32;
			   this->pictureBox36->TabStop = false;
			   this->pictureBox36->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox36_MouseClick);
			   // 
			   // pictureBox35
			   // 
			   this->pictureBox35->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox35->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox35.Image")));
			   this->pictureBox35->Location = System::Drawing::Point(316, 8);
			   this->pictureBox35->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox35->Name = L"pictureBox35";
			   this->pictureBox35->Size = System::Drawing::Size(50, 42);
			   this->pictureBox35->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox35->TabIndex = 31;
			   this->pictureBox35->TabStop = false;
			   this->pictureBox35->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox35_MouseClick);
			   // 
			   // pictureBox34
			   // 
			   this->pictureBox34->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox34->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox34.Image")));
			   this->pictureBox34->Location = System::Drawing::Point(270, 11);
			   this->pictureBox34->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox34->Name = L"pictureBox34";
			   this->pictureBox34->Size = System::Drawing::Size(40, 34);
			   this->pictureBox34->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox34->TabIndex = 30;
			   this->pictureBox34->TabStop = false;
			   // 
			   // label33
			   // 
			   this->label33->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label33->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label33->ForeColor = System::Drawing::Color::White;
			   this->label33->Location = System::Drawing::Point(3, 1);
			   this->label33->Name = L"label33";
			   this->label33->Size = System::Drawing::Size(149, 54);
			   this->label33->TabIndex = 28;
			   this->label33->Text = L"Nombre Rola";
			   this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // pictureBox32
			   // 
			   this->pictureBox32->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox32->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox32.Image")));
			   this->pictureBox32->Location = System::Drawing::Point(163, 11);
			   this->pictureBox32->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox32->Name = L"pictureBox32";
			   this->pictureBox32->Size = System::Drawing::Size(26, 33);
			   this->pictureBox32->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox32->TabIndex = 28;
			   this->pictureBox32->TabStop = false;
			   // 
			   // panel11
			   // 
			   this->panel11->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel11->Controls->Add(this->pictureBox64);
			   this->panel11->Controls->Add(this->label38);
			   this->panel11->Controls->Add(this->label73);
			   this->panel11->Controls->Add(this->pictureBox42);
			   this->panel11->Controls->Add(this->label26);
			   this->panel11->Controls->Add(this->pictureBox31);
			   this->panel11->Controls->Add(this->pictureBox30);
			   this->panel11->Controls->Add(this->pictureBox29);
			   this->panel11->Controls->Add(this->pictureBox27);
			   this->panel11->Controls->Add(this->pictureBox8);
			   this->panel11->Controls->Add(this->label30);
			   this->panel11->Controls->Add(this->label29);
			   this->panel11->Controls->Add(this->label28);
			   this->panel11->Controls->Add(this->label27);
			   this->panel11->Controls->Add(this->panel12);
			   this->panel11->Controls->Add(this->pictureBox6);
			   this->panel11->Location = System::Drawing::Point(0, 0);
			   this->panel11->Margin = System::Windows::Forms::Padding(2);
			   this->panel11->Name = L"panel11";
			   this->panel11->Size = System::Drawing::Size(220, 600);
			   this->panel11->TabIndex = 17;
			   // 
			   // pictureBox64
			   // 
			   this->pictureBox64->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox64->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox64.Image")));
			   this->pictureBox64->Location = System::Drawing::Point(17, 254);
			   this->pictureBox64->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox64->Name = L"pictureBox64";
			   this->pictureBox64->Size = System::Drawing::Size(46, 37);
			   this->pictureBox64->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox64->TabIndex = 79;
			   this->pictureBox64->TabStop = false;
			   // 
			   // label38
			   // 
			   this->label38->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label38->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label38->ForeColor = System::Drawing::Color::White;
			   this->label38->Location = System::Drawing::Point(58, 477);
			   this->label38->Name = L"label38";
			   this->label38->Size = System::Drawing::Size(146, 37);
			   this->label38->TabIndex = 29;
			   this->label38->Text = L"Configuracion";
			   this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label38->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label38_MouseClick);
			   this->label38->MouseEnter += gcnew System::EventHandler(this, &MyForm::label38_MouseEnter);
			   this->label38->MouseLeave += gcnew System::EventHandler(this, &MyForm::label38_MouseLeave);
			   // 
			   // label73
			   // 
			   this->label73->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label73->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label73->ForeColor = System::Drawing::Color::White;
			   this->label73->Location = System::Drawing::Point(51, 254);
			   this->label73->Name = L"label73";
			   this->label73->Size = System::Drawing::Size(151, 37);
			   this->label73->TabIndex = 78;
			   this->label73->Text = L"Estadisticas";
			   this->label73->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label73->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label73_MouseClick);
			   this->label73->MouseEnter += gcnew System::EventHandler(this, &MyForm::label73_MouseEnter);
			   this->label73->MouseLeave += gcnew System::EventHandler(this, &MyForm::label73_MouseLeave);
			   // 
			   // pictureBox42
			   // 
			   this->pictureBox42->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox42->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox42.Image")));
			   this->pictureBox42->Location = System::Drawing::Point(15, 538);
			   this->pictureBox42->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox42->Name = L"pictureBox42";
			   this->pictureBox42->Size = System::Drawing::Size(46, 34);
			   this->pictureBox42->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox42->TabIndex = 28;
			   this->pictureBox42->TabStop = false;
			   // 
			   // label26
			   // 
			   this->label26->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label26->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label26->ForeColor = System::Drawing::Color::White;
			   this->label26->Location = System::Drawing::Point(62, 196);
			   this->label26->Name = L"label26";
			   this->label26->Size = System::Drawing::Size(142, 42);
			   this->label26->TabIndex = 18;
			   this->label26->Text = L"Catalogo ";
			   this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label26->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label26_MouseClick);
			   this->label26->MouseEnter += gcnew System::EventHandler(this, &MyForm::label26_MouseEnter);
			   this->label26->MouseLeave += gcnew System::EventHandler(this, &MyForm::label26_MouseLeave);
			   // 
			   // pictureBox31
			   // 
			   this->pictureBox31->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox31->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox31.Image")));
			   this->pictureBox31->Location = System::Drawing::Point(15, 477);
			   this->pictureBox31->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox31->Name = L"pictureBox31";
			   this->pictureBox31->Size = System::Drawing::Size(46, 37);
			   this->pictureBox31->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			   this->pictureBox31->TabIndex = 27;
			   this->pictureBox31->TabStop = false;
			   // 
			   // pictureBox30
			   // 
			   this->pictureBox30->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox30->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox30.Image")));
			   this->pictureBox30->Location = System::Drawing::Point(15, 420);
			   this->pictureBox30->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox30->Name = L"pictureBox30";
			   this->pictureBox30->Size = System::Drawing::Size(46, 37);
			   this->pictureBox30->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox30->TabIndex = 26;
			   this->pictureBox30->TabStop = false;
			   // 
			   // pictureBox29
			   // 
			   this->pictureBox29->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox29->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox29.Image")));
			   this->pictureBox29->Location = System::Drawing::Point(15, 362);
			   this->pictureBox29->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox29->Name = L"pictureBox29";
			   this->pictureBox29->Size = System::Drawing::Size(46, 37);
			   this->pictureBox29->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox29->TabIndex = 25;
			   this->pictureBox29->TabStop = false;
			   // 
			   // pictureBox27
			   // 
			   this->pictureBox27->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox27->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox27.Image")));
			   this->pictureBox27->Location = System::Drawing::Point(15, 306);
			   this->pictureBox27->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox27->Name = L"pictureBox27";
			   this->pictureBox27->Size = System::Drawing::Size(46, 37);
			   this->pictureBox27->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox27->TabIndex = 24;
			   this->pictureBox27->TabStop = false;
			   // 
			   // pictureBox8
			   // 
			   this->pictureBox8->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox8->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox8.Image")));
			   this->pictureBox8->Location = System::Drawing::Point(15, 195);
			   this->pictureBox8->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox8->Name = L"pictureBox8";
			   this->pictureBox8->Size = System::Drawing::Size(46, 42);
			   this->pictureBox8->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox8->TabIndex = 23;
			   this->pictureBox8->TabStop = false;
			   // 
			   // label30
			   // 
			   this->label30->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label30->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label30->ForeColor = System::Drawing::Color::White;
			   this->label30->Location = System::Drawing::Point(49, 538);
			   this->label30->Name = L"label30";
			   this->label30->Size = System::Drawing::Size(153, 34);
			   this->label30->TabIndex = 22;
			   this->label30->Text = L"Salir";
			   this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label30->Click += gcnew System::EventHandler(this, &MyForm::label30_Click);
			   this->label30->MouseEnter += gcnew System::EventHandler(this, &MyForm::label30_MouseEnter);
			   this->label30->MouseLeave += gcnew System::EventHandler(this, &MyForm::label30_MouseLeave);
			   // 
			   // label29
			   // 
			   this->label29->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label29->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label29->ForeColor = System::Drawing::Color::White;
			   this->label29->Location = System::Drawing::Point(49, 420);
			   this->label29->Name = L"label29";
			   this->label29->Size = System::Drawing::Size(153, 37);
			   this->label29->TabIndex = 21;
			   this->label29->Text = L"Favoritos";
			   this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label29->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label29_MouseClick);
			   this->label29->MouseEnter += gcnew System::EventHandler(this, &MyForm::label29_MouseEnter);
			   this->label29->MouseLeave += gcnew System::EventHandler(this, &MyForm::label29_MouseLeave);
			   // 
			   // label28
			   // 
			   this->label28->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label28->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label28->ForeColor = System::Drawing::Color::White;
			   this->label28->Location = System::Drawing::Point(51, 362);
			   this->label28->Name = L"label28";
			   this->label28->Size = System::Drawing::Size(151, 37);
			   this->label28->TabIndex = 20;
			   this->label28->Text = L"Crear playlist";
			   this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label28->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label28_MouseClick);
			   this->label28->MouseEnter += gcnew System::EventHandler(this, &MyForm::label28_MouseEnter);
			   this->label28->MouseLeave += gcnew System::EventHandler(this, &MyForm::label28_MouseLeave);
			   // 
			   // label27
			   // 
			   this->label27->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label27->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label27->ForeColor = System::Drawing::Color::White;
			   this->label27->Location = System::Drawing::Point(46, 306);
			   this->label27->Name = L"label27";
			   this->label27->Size = System::Drawing::Size(155, 37);
			   this->label27->TabIndex = 19;
			   this->label27->Text = L"Ver Playlists";
			   this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label27->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label27_MouseClick);
			   this->label27->MouseEnter += gcnew System::EventHandler(this, &MyForm::label27_MouseEnter);
			   this->label27->MouseLeave += gcnew System::EventHandler(this, &MyForm::label27_MouseLeave);
			   // 
			   // panel12
			   // 
			   this->panel12->BackColor = System::Drawing::Color::Gray;
			   this->panel12->ForeColor = System::Drawing::Color::Transparent;
			   this->panel12->Location = System::Drawing::Point(14, 171);
			   this->panel12->Margin = System::Windows::Forms::Padding(2);
			   this->panel12->Name = L"panel12";
			   this->panel12->Size = System::Drawing::Size(188, 10);
			   this->panel12->TabIndex = 17;
			   // 
			   // pictureBox6
			   // 
			   this->pictureBox6->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox6.Image")));
			   this->pictureBox6->Location = System::Drawing::Point(42, 41);
			   this->pictureBox6->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox6->Name = L"pictureBox6";
			   this->pictureBox6->Size = System::Drawing::Size(133, 109);
			   this->pictureBox6->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox6->TabIndex = 4;
			   this->pictureBox6->TabStop = false;
			   // 
			   // label22
			   // 
			   this->label22->AutoSize = true;
			   this->label22->Font = (gcnew System::Drawing::Font(L"Segoe UI", 19.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label22->ForeColor = System::Drawing::Color::White;
			   this->label22->Location = System::Drawing::Point(238, 26);
			   this->label22->Name = L"label22";
			   this->label22->Size = System::Drawing::Size(300, 37);
			   this->label22->TabIndex = 3;
			   this->label22->Text = L"Bienvenido de nuevo, ";
			   // 
			   // pictureBox7
			   // 
			   this->pictureBox7->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox7.Image")));
			   this->pictureBox7->Location = System::Drawing::Point(642, 25);
			   this->pictureBox7->Name = L"pictureBox7";
			   this->pictureBox7->Size = System::Drawing::Size(44, 37);
			   this->pictureBox7->TabIndex = 2;
			   this->pictureBox7->TabStop = false;
			   // 
			   // label24
			   // 
			   this->label24->AutoSize = true;
			   this->label24->Font = (gcnew System::Drawing::Font(L"Segoe UI", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label24->ForeColor = System::Drawing::Color::White;
			   this->label24->Location = System::Drawing::Point(692, 19);
			   this->label24->Name = L"label24";
			   this->label24->Size = System::Drawing::Size(135, 45);
			   this->label24->TabIndex = 1;
			   this->label24->Text = L"TuneUp";
			   // 
			   // panelArtista
			   // 
			   this->panelArtista->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->panelArtista->Controls->Add(this->PanelConfi);
			   this->panelArtista->Controls->Add(this->panelStats);
			   this->panelArtista->Controls->Add(this->PanelSubir);
			   this->panelArtista->Controls->Add(this->PanelEliminar);
			   this->panelArtista->Controls->Add(this->panelDisco);
			   this->panelArtista->Controls->Add(this->PanelEditar);
			   this->panelArtista->Controls->Add(this->panel6);
			   this->panelArtista->Controls->Add(this->panel8);
			   this->panelArtista->Controls->Add(this->pictureBox57);
			   this->panelArtista->Controls->Add(this->label59);
			   this->panelArtista->Controls->Add(this->label16);
			   this->panelArtista->Location = System::Drawing::Point(0, 0);
			   this->panelArtista->Margin = System::Windows::Forms::Padding(2);
			   this->panelArtista->Name = L"panelArtista";
			   this->panelArtista->Size = System::Drawing::Size(1396, 736);
			   this->panelArtista->TabIndex = 9;
			   // 
			   // PanelConfi
			   // 
			   this->PanelConfi->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelConfi->Controls->Add(this->panel15);
			   this->PanelConfi->Controls->Add(this->panel13);
			   this->PanelConfi->Controls->Add(this->panel10);
			   this->PanelConfi->Controls->Add(this->label70);
			   this->PanelConfi->Controls->Add(this->label71);
			   this->PanelConfi->Controls->Add(this->comboBox2);
			   this->PanelConfi->Controls->Add(this->textBox21);
			   this->PanelConfi->Controls->Add(this->textBox22);
			   this->PanelConfi->Controls->Add(this->textBox23);
			   this->PanelConfi->Controls->Add(this->textBox24);
			   this->PanelConfi->Controls->Add(this->textBox25);
			   this->PanelConfi->Controls->Add(this->textBox26);
			   this->PanelConfi->Controls->Add(this->button10);
			   this->PanelConfi->Controls->Add(this->button11);
			   this->PanelConfi->Controls->Add(this->label72);
			   this->PanelConfi->Controls->Add(this->label76);
			   this->PanelConfi->Controls->Add(this->label77);
			   this->PanelConfi->Controls->Add(this->label78);
			   this->PanelConfi->Controls->Add(this->label79);
			   this->PanelConfi->ForeColor = System::Drawing::Color::White;
			   this->PanelConfi->Location = System::Drawing::Point(288, 79);
			   this->PanelConfi->Margin = System::Windows::Forms::Padding(2);
			   this->PanelConfi->Name = L"PanelConfi";
			   this->PanelConfi->Size = System::Drawing::Size(1096, 517);
			   this->PanelConfi->TabIndex = 65;
			   // 
			   // panel15
			   // 
			   this->panel15->BackColor = System::Drawing::Color::Gray;
			   this->panel15->Location = System::Drawing::Point(351, 90);
			   this->panel15->Name = L"panel15";
			   this->panel15->Size = System::Drawing::Size(10, 395);
			   this->panel15->TabIndex = 90;
			   // 
			   // panel13
			   // 
			   this->panel13->BackColor = System::Drawing::Color::Gray;
			   this->panel13->Controls->Add(this->label94);
			   this->panel13->Location = System::Drawing::Point(16, 22);
			   this->panel13->Name = L"panel13";
			   this->panel13->Size = System::Drawing::Size(1065, 71);
			   this->panel13->TabIndex = 89;
			   // 
			   // label94
			   // 
			   this->label94->AutoSize = true;
			   this->label94->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label94->ForeColor = System::Drawing::Color::White;
			   this->label94->Location = System::Drawing::Point(431, 16);
			   this->label94->Name = L"label94";
			   this->label94->Size = System::Drawing::Size(172, 32);
			   this->label94->TabIndex = 66;
			   this->label94->Text = L"Edite su Perfil";
			   // 
			   // panel10
			   // 
			   this->panel10->BackColor = System::Drawing::Color::Gray;
			   this->panel10->Controls->Add(this->pictureBox63);
			   this->panel10->Controls->Add(this->button9);
			   this->panel10->Location = System::Drawing::Point(750, 112);
			   this->panel10->Name = L"panel10";
			   this->panel10->Size = System::Drawing::Size(331, 373);
			   this->panel10->TabIndex = 88;
			   // 
			   // pictureBox63
			   // 
			   this->pictureBox63->BackColor = System::Drawing::Color::Transparent;
			   this->pictureBox63->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox63.Image")));
			   this->pictureBox63->Location = System::Drawing::Point(62, 29);
			   this->pictureBox63->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox63->Name = L"pictureBox63";
			   this->pictureBox63->Size = System::Drawing::Size(228, 170);
			   this->pictureBox63->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox63->TabIndex = 32;
			   this->pictureBox63->TabStop = false;
			   // 
			   // button9
			   // 
			   this->button9->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->button9->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button9->FlatAppearance->BorderSize = 0;
			   this->button9->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button9->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button9->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->button9->Location = System::Drawing::Point(62, 234);
			   this->button9->Margin = System::Windows::Forms::Padding(2);
			   this->button9->Name = L"button9";
			   this->button9->Size = System::Drawing::Size(228, 40);
			   this->button9->TabIndex = 75;
			   this->button9->Text = L"Seleccionar Imagen";
			   this->button9->UseVisualStyleBackColor = false;
			   this->button9->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::button9_MouseClick);
			   // 
			   // label70
			   // 
			   this->label70->BackColor = System::Drawing::Color::Transparent;
			   this->label70->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label70->ForeColor = System::Drawing::Color::White;
			   this->label70->Location = System::Drawing::Point(17, 109);
			   this->label70->Name = L"label70";
			   this->label70->Size = System::Drawing::Size(252, 23);
			   this->label70->TabIndex = 87;
			   this->label70->Text = L"Usuario:";
			   // 
			   // label71
			   // 
			   this->label71->BackColor = System::Drawing::Color::Transparent;
			   this->label71->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label71->ForeColor = System::Drawing::Color::White;
			   this->label71->Location = System::Drawing::Point(394, 179);
			   this->label71->Name = L"label71";
			   this->label71->Size = System::Drawing::Size(252, 19);
			   this->label71->TabIndex = 65;
			   this->label71->Text = L"Confirmar:                                                     ";
			   // 
			   // comboBox2
			   // 
			   this->comboBox2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->comboBox2->FormattingEnabled = true;
			   this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				   L"pop", L"Corridos", L"cristianos", L"electrónica",
					   L"reguetón", L"rock", L"clasicas"
			   });
			   this->comboBox2->Location = System::Drawing::Point(397, 272);
			   this->comboBox2->Margin = System::Windows::Forms::Padding(2);
			   this->comboBox2->Name = L"comboBox2";
			   this->comboBox2->Size = System::Drawing::Size(335, 21);
			   this->comboBox2->TabIndex = 78;
			   // 
			   // textBox21
			   // 
			   this->textBox21->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox21->Location = System::Drawing::Point(397, 203);
			   this->textBox21->Margin = System::Windows::Forms::Padding(2);
			   this->textBox21->Multiline = true;
			   this->textBox21->Name = L"textBox21";
			   this->textBox21->PasswordChar = '*';
			   this->textBox21->Size = System::Drawing::Size(334, 29);
			   this->textBox21->TabIndex = 77;
			   this->textBox21->UseSystemPasswordChar = true;
			   // 
			   // textBox22
			   // 
			   this->textBox22->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox22->Location = System::Drawing::Point(397, 134);
			   this->textBox22->Margin = System::Windows::Forms::Padding(2);
			   this->textBox22->Multiline = true;
			   this->textBox22->Name = L"textBox22";
			   this->textBox22->PasswordChar = '*';
			   this->textBox22->Size = System::Drawing::Size(334, 28);
			   this->textBox22->TabIndex = 76;
			   this->textBox22->UseSystemPasswordChar = true;
			   // 
			   // textBox23
			   // 
			   this->textBox23->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox23->Location = System::Drawing::Point(17, 329);
			   this->textBox23->Margin = System::Windows::Forms::Padding(2);
			   this->textBox23->Multiline = true;
			   this->textBox23->Name = L"textBox23";
			   this->textBox23->Size = System::Drawing::Size(299, 26);
			   this->textBox23->TabIndex = 74;
			   // 
			   // textBox24
			   // 
			   this->textBox24->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox24->Location = System::Drawing::Point(16, 269);
			   this->textBox24->Margin = System::Windows::Forms::Padding(2);
			   this->textBox24->Multiline = true;
			   this->textBox24->Name = L"textBox24";
			   this->textBox24->Size = System::Drawing::Size(299, 28);
			   this->textBox24->TabIndex = 73;
			   // 
			   // textBox25
			   // 
			   this->textBox25->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox25->Location = System::Drawing::Point(16, 203);
			   this->textBox25->Margin = System::Windows::Forms::Padding(2);
			   this->textBox25->Multiline = true;
			   this->textBox25->Name = L"textBox25";
			   this->textBox25->Size = System::Drawing::Size(299, 29);
			   this->textBox25->TabIndex = 72;
			   // 
			   // textBox26
			   // 
			   this->textBox26->BackColor = System::Drawing::Color::White;
			   this->textBox26->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox26->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox26->ForeColor = System::Drawing::Color::Black;
			   this->textBox26->Location = System::Drawing::Point(17, 134);
			   this->textBox26->Margin = System::Windows::Forms::Padding(2);
			   this->textBox26->Multiline = true;
			   this->textBox26->Name = L"textBox26";
			   this->textBox26->Size = System::Drawing::Size(299, 27);
			   this->textBox26->TabIndex = 71;
			   // 
			   // button10
			   // 
			   this->button10->BackColor = System::Drawing::Color::LightGray;
			   this->button10->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button10->FlatAppearance->BorderSize = 0;
			   this->button10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button10->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button10->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->button10->Location = System::Drawing::Point(397, 443);
			   this->button10->Margin = System::Windows::Forms::Padding(2);
			   this->button10->Name = L"button10";
			   this->button10->Padding = System::Windows::Forms::Padding(10, 5, 10, 5);
			   this->button10->Size = System::Drawing::Size(334, 42);
			   this->button10->TabIndex = 70;
			   this->button10->Text = L"Elimnar Cuenta";
			   this->button10->UseVisualStyleBackColor = false;
			   this->button10->Click += gcnew System::EventHandler(this, &MyForm::button10_Click);
			   // 
			   // button11
			   // 
			   this->button11->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->button11->Cursor = System::Windows::Forms::Cursors::Hand;
			   this->button11->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->button11->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->button11->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->button11->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			   this->button11->Location = System::Drawing::Point(16, 443);
			   this->button11->Margin = System::Windows::Forms::Padding(2);
			   this->button11->Name = L"button11";
			   this->button11->Padding = System::Windows::Forms::Padding(10, 5, 10, 5);
			   this->button11->Size = System::Drawing::Size(300, 42);
			   this->button11->TabIndex = 69;
			   this->button11->Text = L"Editar";
			   this->button11->UseVisualStyleBackColor = false;
			   this->button11->Click += gcnew System::EventHandler(this, &MyForm::button11_Click);
			   // 
			   // label72
			   // 
			   this->label72->BackColor = System::Drawing::Color::Transparent;
			   this->label72->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label72->ForeColor = System::Drawing::Color::White;
			   this->label72->Location = System::Drawing::Point(396, 247);
			   this->label72->Name = L"label72";
			   this->label72->Size = System::Drawing::Size(250, 27);
			   this->label72->TabIndex = 68;
			   this->label72->Text = L"Genero:                                                          ";
			   // 
			   // label76
			   // 
			   this->label76->BackColor = System::Drawing::Color::Transparent;
			   this->label76->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label76->ForeColor = System::Drawing::Color::White;
			   this->label76->Location = System::Drawing::Point(394, 111);
			   this->label76->Name = L"label76";
			   this->label76->Size = System::Drawing::Size(252, 20);
			   this->label76->TabIndex = 63;
			   this->label76->Text = L"Contraseña:                                                   ";
			   // 
			   // label77
			   // 
			   this->label77->BackColor = System::Drawing::Color::Transparent;
			   this->label77->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label77->ForeColor = System::Drawing::Color::White;
			   this->label77->Location = System::Drawing::Point(17, 305);
			   this->label77->Name = L"label77";
			   this->label77->Size = System::Drawing::Size(251, 22);
			   this->label77->TabIndex = 62;
			   this->label77->Text = L"Nombre:                                                       ";
			   // 
			   // label78
			   // 
			   this->label78->BackColor = System::Drawing::Color::Transparent;
			   this->label78->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label78->ForeColor = System::Drawing::Color::White;
			   this->label78->Location = System::Drawing::Point(16, 245);
			   this->label78->Name = L"label78";
			   this->label78->Size = System::Drawing::Size(251, 25);
			   this->label78->TabIndex = 61;
			   this->label78->Text = L"Nombre Artistico:                                         ";
			   // 
			   // label79
			   // 
			   this->label79->BackColor = System::Drawing::Color::Transparent;
			   this->label79->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label79->ForeColor = System::Drawing::Color::White;
			   this->label79->Location = System::Drawing::Point(17, 176);
			   this->label79->Name = L"label79";
			   this->label79->Size = System::Drawing::Size(251, 19);
			   this->label79->TabIndex = 60;
			   this->label79->Text = L"Correo Electronico:                                        ";
			   // 
			   // panelStats
			   // 
			   this->panelStats->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panelStats->Controls->Add(this->label104);
			   this->panelStats->Controls->Add(this->label66);
			   this->panelStats->Controls->Add(this->label65);
			   this->panelStats->Controls->Add(this->label60);
			   this->panelStats->Controls->Add(this->listBox17);
			   this->panelStats->Controls->Add(this->listBox14);
			   this->panelStats->Controls->Add(this->listBox13);
			   this->panelStats->Controls->Add(this->panel24);
			   this->panelStats->ForeColor = System::Drawing::Color::White;
			   this->panelStats->Location = System::Drawing::Point(288, 77);
			   this->panelStats->Margin = System::Windows::Forms::Padding(2);
			   this->panelStats->Name = L"panelStats";
			   this->panelStats->Size = System::Drawing::Size(1096, 516);
			   this->panelStats->TabIndex = 57;
			   // 
			   // label104
			   // 
			   this->label104->AutoSize = true;
			   this->label104->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label104->ForeColor = System::Drawing::Color::White;
			   this->label104->Location = System::Drawing::Point(13, 5);
			   this->label104->Name = L"label104";
			   this->label104->Size = System::Drawing::Size(145, 32);
			   this->label104->TabIndex = 66;
			   this->label104->Text = L"Estadisticas";
			   // 
			   // label66
			   // 
			   this->label66->AutoSize = true;
			   this->label66->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label66->ForeColor = System::Drawing::Color::White;
			   this->label66->Location = System::Drawing::Point(831, 53);
			   this->label66->Name = L"label66";
			   this->label66->Size = System::Drawing::Size(169, 21);
			   this->label66->TabIndex = 60;
			   this->label66->Text = L"Usuarios mas activos";
			   this->label66->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label65
			   // 
			   this->label65->AutoSize = true;
			   this->label65->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label65->ForeColor = System::Drawing::Color::White;
			   this->label65->Location = System::Drawing::Point(429, 51);
			   this->label65->Name = L"label65";
			   this->label65->Size = System::Drawing::Size(224, 21);
			   this->label65->TabIndex = 59;
			   this->label65->Text = L"Canciones mejor Calificadas";
			   this->label65->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label60
			   // 
			   this->label60->AutoSize = true;
			   this->label60->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label60->ForeColor = System::Drawing::Color::White;
			   this->label60->Location = System::Drawing::Point(55, 52);
			   this->label60->Name = L"label60";
			   this->label60->Size = System::Drawing::Size(214, 21);
			   this->label60->TabIndex = 58;
			   this->label60->Text = L"Canciones mas Escuchadas";
			   this->label60->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // listBox17
			   // 
			   this->listBox17->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox17->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox17->ForeColor = System::Drawing::Color::White;
			   this->listBox17->FormattingEnabled = true;
			   this->listBox17->ItemHeight = 25;
			   this->listBox17->Location = System::Drawing::Point(755, 80);
			   this->listBox17->Margin = System::Windows::Forms::Padding(2);
			   this->listBox17->Name = L"listBox17";
			   this->listBox17->Size = System::Drawing::Size(319, 154);
			   this->listBox17->TabIndex = 52;
			   // 
			   // listBox14
			   // 
			   this->listBox14->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox14->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox14->ForeColor = System::Drawing::Color::White;
			   this->listBox14->FormattingEnabled = true;
			   this->listBox14->ItemHeight = 25;
			   this->listBox14->Location = System::Drawing::Point(19, 82);
			   this->listBox14->Margin = System::Windows::Forms::Padding(2);
			   this->listBox14->Name = L"listBox14";
			   this->listBox14->Size = System::Drawing::Size(318, 154);
			   this->listBox14->TabIndex = 50;
			   // 
			   // listBox13
			   // 
			   this->listBox13->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox13->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox13->ForeColor = System::Drawing::Color::White;
			   this->listBox13->FormattingEnabled = true;
			   this->listBox13->ItemHeight = 25;
			   this->listBox13->Location = System::Drawing::Point(390, 80);
			   this->listBox13->Margin = System::Windows::Forms::Padding(2);
			   this->listBox13->Name = L"listBox13";
			   this->listBox13->Size = System::Drawing::Size(317, 154);
			   this->listBox13->TabIndex = 48;
			   // 
			   // panel24
			   // 
			   this->panel24->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)),
				   static_cast<System::Int32>(static_cast<System::Byte>(40)));
			   this->panel24->Controls->Add(this->dataGridView2);
			   this->panel24->Controls->Add(this->label67);
			   this->panel24->Controls->Add(this->dataGridView1);
			   this->panel24->Controls->Add(this->label68);
			   this->panel24->Location = System::Drawing::Point(21, 250);
			   this->panel24->Name = L"panel24";
			   this->panel24->Size = System::Drawing::Size(1055, 265);
			   this->panel24->TabIndex = 65;
			   // 
			   // dataGridView2
			   // 
			   this->dataGridView2->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			   this->dataGridView2->Location = System::Drawing::Point(19, 54);
			   this->dataGridView2->Margin = System::Windows::Forms::Padding(2);
			   this->dataGridView2->Name = L"dataGridView2";
			   this->dataGridView2->RowHeadersWidth = 51;
			   this->dataGridView2->RowTemplate->Height = 24;
			   this->dataGridView2->Size = System::Drawing::Size(399, 181);
			   this->dataGridView2->TabIndex = 64;
			   // 
			   // label67
			   // 
			   this->label67->AutoSize = true;
			   this->label67->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label67->ForeColor = System::Drawing::Color::White;
			   this->label67->Location = System::Drawing::Point(72, 21);
			   this->label67->Name = L"label67";
			   this->label67->Size = System::Drawing::Size(241, 21);
			   this->label67->TabIndex = 61;
			   this->label67->Text = L"Distribucion de Calificaciones ";
			   this->label67->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // dataGridView1
			   // 
			   this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			   this->dataGridView1->Location = System::Drawing::Point(637, 54);
			   this->dataGridView1->Margin = System::Windows::Forms::Padding(2);
			   this->dataGridView1->Name = L"dataGridView1";
			   this->dataGridView1->RowHeadersWidth = 51;
			   this->dataGridView1->RowTemplate->Height = 24;
			   this->dataGridView1->Size = System::Drawing::Size(399, 181);
			   this->dataGridView1->TabIndex = 62;
			   // 
			   // label68
			   // 
			   this->label68->AutoSize = true;
			   this->label68->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label68->ForeColor = System::Drawing::Color::White;
			   this->label68->Location = System::Drawing::Point(770, 21);
			   this->label68->Name = L"label68";
			   this->label68->Size = System::Drawing::Size(154, 21);
			   this->label68->TabIndex = 63;
			   this->label68->Text = L"Tiempos promedio";
			   this->label68->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // PanelSubir
			   // 
			   this->PanelSubir->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelSubir->Controls->Add(this->label58);
			   this->PanelSubir->Controls->Add(this->textBox14);
			   this->PanelSubir->Controls->Add(this->label47);
			   this->PanelSubir->Controls->Add(this->lblConteoArchivos);
			   this->PanelSubir->Controls->Add(this->btnAgregarAudios);
			   this->PanelSubir->Controls->Add(this->checkBox3);
			   this->PanelSubir->Controls->Add(this->panel17);
			   this->PanelSubir->Controls->Add(this->label96);
			   this->PanelSubir->Controls->Add(this->panel16);
			   this->PanelSubir->Controls->Add(this->comboBox3);
			   this->PanelSubir->Controls->Add(this->label86);
			   this->PanelSubir->Controls->Add(this->textBox15);
			   this->PanelSubir->Controls->Add(this->label85);
			   this->PanelSubir->Controls->Add(this->comboBox1);
			   this->PanelSubir->Controls->Add(this->label84);
			   this->PanelSubir->Controls->Add(this->textBox7);
			   this->PanelSubir->Controls->Add(this->label49);
			   this->PanelSubir->Controls->Add(this->label41);
			   this->PanelSubir->Controls->Add(this->pictureBox58);
			   this->PanelSubir->ForeColor = System::Drawing::Color::White;
			   this->PanelSubir->Location = System::Drawing::Point(288, 77);
			   this->PanelSubir->Margin = System::Windows::Forms::Padding(2);
			   this->PanelSubir->Name = L"PanelSubir";
			   this->PanelSubir->Size = System::Drawing::Size(1096, 519);
			   this->PanelSubir->TabIndex = 40;
			   // 
			   // label58
			   // 
			   this->label58->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label58->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label58->ForeColor = System::Drawing::Color::White;
			   this->label58->Location = System::Drawing::Point(195, 452);
			   this->label58->Name = L"label58";
			   this->label58->Size = System::Drawing::Size(214, 52);
			   this->label58->TabIndex = 72;
			   this->label58->Text = L"Finalizar";
			   this->label58->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label58->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label58_MouseClick);
			   this->label58->MouseEnter += gcnew System::EventHandler(this, &MyForm::label58_MouseEnter);
			   this->label58->MouseLeave += gcnew System::EventHandler(this, &MyForm::label58_MouseLeave);
			   // 
			   // textBox14
			   // 
			   this->textBox14->BackColor = System::Drawing::Color::DimGray;
			   this->textBox14->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox14->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox14->Location = System::Drawing::Point(20, 281);
			   this->textBox14->Margin = System::Windows::Forms::Padding(2);
			   this->textBox14->Multiline = true;
			   this->textBox14->Name = L"textBox14";
			   this->textBox14->Size = System::Drawing::Size(389, 27);
			   this->textBox14->TabIndex = 71;
			   // 
			   // label47
			   // 
			   this->label47->AutoSize = true;
			   this->label47->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label47->ForeColor = System::Drawing::Color::White;
			   this->label47->Location = System::Drawing::Point(19, 250);
			   this->label47->Name = L"label47";
			   this->label47->Size = System::Drawing::Size(159, 19);
			   this->label47->TabIndex = 70;
			   this->label47->Text = L"Nombre de la Cancion";
			   // 
			   // lblConteoArchivos
			   // 
			   this->lblConteoArchivos->AutoSize = true;
			   this->lblConteoArchivos->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->lblConteoArchivos->ForeColor = System::Drawing::Color::White;
			   this->lblConteoArchivos->Location = System::Drawing::Point(330, 144);
			   this->lblConteoArchivos->Name = L"lblConteoArchivos";
			   this->lblConteoArchivos->Size = System::Drawing::Size(79, 19);
			   this->lblConteoArchivos->TabIndex = 69;
			   this->lblConteoArchivos->Text = L"0 Archivos";
			   // 
			   // btnAgregarAudios
			   // 
			   this->btnAgregarAudios->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->btnAgregarAudios->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->btnAgregarAudios->ForeColor = System::Drawing::Color::White;
			   this->btnAgregarAudios->Location = System::Drawing::Point(21, 134);
			   this->btnAgregarAudios->Name = L"btnAgregarAudios";
			   this->btnAgregarAudios->Size = System::Drawing::Size(199, 37);
			   this->btnAgregarAudios->TabIndex = 68;
			   this->btnAgregarAudios->Text = L"Agregar audio(s)";
			   this->btnAgregarAudios->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // checkBox3
			   // 
			   this->checkBox3->AutoSize = true;
			   this->checkBox3->Location = System::Drawing::Point(339, 55);
			   this->checkBox3->Name = L"checkBox3";
			   this->checkBox3->Size = System::Drawing::Size(70, 17);
			   this->checkBox3->TabIndex = 68;
			   this->checkBox3->Text = L"Es Single";
			   this->checkBox3->UseVisualStyleBackColor = true;
			   this->checkBox3->CheckedChanged += gcnew System::EventHandler(this, &MyForm::checkBox3_CheckedChanged);
			   // 
			   // panel17
			   // 
			   this->panel17->BackColor = System::Drawing::Color::Silver;
			   this->panel17->Location = System::Drawing::Point(456, 2);
			   this->panel17->Name = L"panel17";
			   this->panel17->Size = System::Drawing::Size(10, 517);
			   this->panel17->TabIndex = 67;
			   // 
			   // label96
			   // 
			   this->label96->AutoSize = true;
			   this->label96->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label96->ForeColor = System::Drawing::Color::White;
			   this->label96->Location = System::Drawing::Point(15, 9);
			   this->label96->Name = L"label96";
			   this->label96->Size = System::Drawing::Size(199, 32);
			   this->label96->TabIndex = 66;
			   this->label96->Text = L"Datos Generales";
			   // 
			   // panel16
			   // 
			   this->panel16->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel16->Controls->Add(this->label97);
			   this->panel16->Controls->Add(this->listBox6);
			   this->panel16->Controls->Add(this->label95);
			   this->panel16->Controls->Add(this->label48);
			   this->panel16->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->panel16->Location = System::Drawing::Point(470, 0);
			   this->panel16->Name = L"panel16";
			   this->panel16->Size = System::Drawing::Size(623, 518);
			   this->panel16->TabIndex = 62;
			   // 
			   // label97
			   // 
			   this->label97->AutoSize = true;
			   this->label97->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label97->ForeColor = System::Drawing::Color::White;
			   this->label97->Location = System::Drawing::Point(16, 10);
			   this->label97->Name = L"label97";
			   this->label97->Size = System::Drawing::Size(201, 32);
			   this->label97->TabIndex = 67;
			   this->label97->Text = L"Pistas a publicar";
			   // 
			   // listBox6
			   // 
			   this->listBox6->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox6->ForeColor = System::Drawing::SystemColors::Menu;
			   this->listBox6->FormattingEnabled = true;
			   this->listBox6->ItemHeight = 25;
			   this->listBox6->Location = System::Drawing::Point(104, 62);
			   this->listBox6->Margin = System::Windows::Forms::Padding(2);
			   this->listBox6->Name = L"listBox6";
			   this->listBox6->Size = System::Drawing::Size(481, 354);
			   this->listBox6->TabIndex = 47;
			   this->listBox6->DrawItem += gcnew System::Windows::Forms::DrawItemEventHandler(this, &MyForm::listBox6_DrawItem_1);
			   this->listBox6->MeasureItem += gcnew System::Windows::Forms::MeasureItemEventHandler(this, &MyForm::listBox6_MeasureItem_1);
			   this->listBox6->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox6_SelectedIndexChanged_1);
			   // 
			   // label95
			   // 
			   this->label95->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label95->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label95->ForeColor = System::Drawing::Color::White;
			   this->label95->Location = System::Drawing::Point(443, 454);
			   this->label95->Name = L"label95";
			   this->label95->Size = System::Drawing::Size(142, 37);
			   this->label95->TabIndex = 52;
			   this->label95->Text = L"Cancelar";
			   this->label95->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label95->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label95_MouseClick);
			   // 
			   // label48
			   // 
			   this->label48->BackColor = System::Drawing::Color::PaleVioletRed;
			   this->label48->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label48->ForeColor = System::Drawing::Color::White;
			   this->label48->Location = System::Drawing::Point(104, 446);
			   this->label48->Name = L"label48";
			   this->label48->Size = System::Drawing::Size(142, 37);
			   this->label48->TabIndex = 51;
			   this->label48->Text = L"Publicar";
			   this->label48->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label48->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label48_MouseClick);
			   this->label48->MouseEnter += gcnew System::EventHandler(this, &MyForm::label48_MouseEnter);
			   this->label48->MouseLeave += gcnew System::EventHandler(this, &MyForm::label48_MouseLeave);
			   // 
			   // comboBox3
			   // 
			   this->comboBox3->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->comboBox3->FormattingEnabled = true;
			   this->comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
				   L"playlist", L"recomendado", L"favorito", L"infantil",
					   L"instrumental"
			   });
			   this->comboBox3->Location = System::Drawing::Point(243, 211);
			   this->comboBox3->Margin = System::Windows::Forms::Padding(2);
			   this->comboBox3->Name = L"comboBox3";
			   this->comboBox3->Size = System::Drawing::Size(166, 21);
			   this->comboBox3->TabIndex = 61;
			   // 
			   // label86
			   // 
			   this->label86->AutoSize = true;
			   this->label86->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label86->ForeColor = System::Drawing::Color::White;
			   this->label86->Location = System::Drawing::Point(239, 182);
			   this->label86->Name = L"label86";
			   this->label86->Size = System::Drawing::Size(75, 19);
			   this->label86->TabIndex = 60;
			   this->label86->Text = L"Categoria";
			   // 
			   // textBox15
			   // 
			   this->textBox15->BackColor = System::Drawing::Color::DimGray;
			   this->textBox15->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox15->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox15->Location = System::Drawing::Point(18, 344);
			   this->textBox15->Margin = System::Windows::Forms::Padding(2);
			   this->textBox15->Multiline = true;
			   this->textBox15->Name = L"textBox15";
			   this->textBox15->Size = System::Drawing::Size(391, 38);
			   this->textBox15->TabIndex = 59;
			   // 
			   // label85
			   // 
			   this->label85->AutoSize = true;
			   this->label85->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label85->ForeColor = System::Drawing::Color::White;
			   this->label85->Location = System::Drawing::Point(17, 318);
			   this->label85->Name = L"label85";
			   this->label85->Size = System::Drawing::Size(161, 19);
			   this->label85->TabIndex = 58;
			   this->label85->Text = L"Descripcion (Opcional)";
			   // 
			   // comboBox1
			   // 
			   this->comboBox1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->comboBox1->FormattingEnabled = true;
			   this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				   L"pop", L"Corridos", L"cristianos", L"electrónica",
					   L"reguetón", L"rock", L"clasicas"
			   });
			   this->comboBox1->Location = System::Drawing::Point(21, 210);
			   this->comboBox1->Margin = System::Windows::Forms::Padding(2);
			   this->comboBox1->Name = L"comboBox1";
			   this->comboBox1->Size = System::Drawing::Size(199, 21);
			   this->comboBox1->TabIndex = 57;
			   // 
			   // label84
			   // 
			   this->label84->AutoSize = true;
			   this->label84->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label84->ForeColor = System::Drawing::Color::White;
			   this->label84->Location = System::Drawing::Point(19, 180);
			   this->label84->Name = L"label84";
			   this->label84->Size = System::Drawing::Size(58, 19);
			   this->label84->TabIndex = 56;
			   this->label84->Text = L"Genero";
			   // 
			   // textBox7
			   // 
			   this->textBox7->BackColor = System::Drawing::Color::DimGray;
			   this->textBox7->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox7->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox7->Location = System::Drawing::Point(21, 86);
			   this->textBox7->Margin = System::Windows::Forms::Padding(2);
			   this->textBox7->Multiline = true;
			   this->textBox7->Name = L"textBox7";
			   this->textBox7->Size = System::Drawing::Size(388, 27);
			   this->textBox7->TabIndex = 53;
			   // 
			   // label49
			   // 
			   this->label49->AutoSize = true;
			   this->label49->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label49->ForeColor = System::Drawing::Color::White;
			   this->label49->Location = System::Drawing::Point(19, 52);
			   this->label49->Name = L"label49";
			   this->label49->Size = System::Drawing::Size(138, 19);
			   this->label49->TabIndex = 52;
			   this->label49->Text = L"Nombre del Album";
			   // 
			   // label41
			   // 
			   this->label41->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label41->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label41->ForeColor = System::Drawing::Color::White;
			   this->label41->Location = System::Drawing::Point(195, 391);
			   this->label41->Name = L"label41";
			   this->label41->Size = System::Drawing::Size(214, 49);
			   this->label41->TabIndex = 44;
			   this->label41->Text = L"Seleccionar Imagen";
			   this->label41->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label41->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label41_MouseClick);
			   this->label41->MouseEnter += gcnew System::EventHandler(this, &MyForm::label41_MouseEnter);
			   this->label41->MouseLeave += gcnew System::EventHandler(this, &MyForm::label41_MouseLeave);
			   // 
			   // pictureBox58
			   // 
			   this->pictureBox58->BackColor = System::Drawing::Color::Transparent;
			   this->pictureBox58->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox58.Image")));
			   this->pictureBox58->Location = System::Drawing::Point(17, 393);
			   this->pictureBox58->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox58->Name = L"pictureBox58";
			   this->pictureBox58->Size = System::Drawing::Size(170, 119);
			   this->pictureBox58->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox58->TabIndex = 45;
			   this->pictureBox58->TabStop = false;
			   // 
			   // PanelEliminar
			   // 
			   this->PanelEliminar->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelEliminar->Controls->Add(this->label101);
			   this->PanelEliminar->Controls->Add(this->label100);
			   this->PanelEliminar->Controls->Add(this->label99);
			   this->PanelEliminar->Controls->Add(this->listBox9);
			   this->PanelEliminar->Controls->Add(this->label64);
			   this->PanelEliminar->Controls->Add(this->listBox10);
			   this->PanelEliminar->Controls->Add(this->panel22);
			   this->PanelEliminar->ForeColor = System::Drawing::Color::White;
			   this->PanelEliminar->Location = System::Drawing::Point(288, 77);
			   this->PanelEliminar->Margin = System::Windows::Forms::Padding(2);
			   this->PanelEliminar->Name = L"PanelEliminar";
			   this->PanelEliminar->Size = System::Drawing::Size(1096, 518);
			   this->PanelEliminar->TabIndex = 55;
			   // 
			   // label101
			   // 
			   this->label101->AutoSize = true;
			   this->label101->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label101->ForeColor = System::Drawing::Color::White;
			   this->label101->Location = System::Drawing::Point(610, 77);
			   this->label101->Name = L"label101";
			   this->label101->Size = System::Drawing::Size(130, 32);
			   this->label101->TabIndex = 51;
			   this->label101->Text = L"Canciones";
			   // 
			   // label100
			   // 
			   this->label100->AutoSize = true;
			   this->label100->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label100->ForeColor = System::Drawing::Color::White;
			   this->label100->Location = System::Drawing::Point(32, 78);
			   this->label100->Name = L"label100";
			   this->label100->Size = System::Drawing::Size(206, 32);
			   this->label100->TabIndex = 50;
			   this->label100->Text = L"Albumes/Singles";
			   // 
			   // label99
			   // 
			   this->label99->AutoSize = true;
			   this->label99->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label99->ForeColor = System::Drawing::Color::White;
			   this->label99->Location = System::Drawing::Point(11, 14);
			   this->label99->Name = L"label99";
			   this->label99->Size = System::Drawing::Size(207, 32);
			   this->label99->TabIndex = 49;
			   this->label99->Text = L"Eliminar Cancion";
			   // 
			   // listBox9
			   // 
			   this->listBox9->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox9->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox9->ForeColor = System::Drawing::Color::White;
			   this->listBox9->FormattingEnabled = true;
			   this->listBox9->ItemHeight = 25;
			   this->listBox9->Location = System::Drawing::Point(17, 125);
			   this->listBox9->Margin = System::Windows::Forms::Padding(2);
			   this->listBox9->Name = L"listBox9";
			   this->listBox9->Size = System::Drawing::Size(488, 279);
			   this->listBox9->TabIndex = 48;
			   // 
			   // label64
			   // 
			   this->label64->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label64->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label64->ForeColor = System::Drawing::Color::White;
			   this->label64->Location = System::Drawing::Point(349, 457);
			   this->label64->Name = L"label64";
			   this->label64->Size = System::Drawing::Size(398, 37);
			   this->label64->TabIndex = 44;
			   this->label64->Text = L"Eliminar";
			   this->label64->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label64->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label64_MouseClick);
			   this->label64->MouseEnter += gcnew System::EventHandler(this, &MyForm::label64_MouseEnter);
			   this->label64->MouseLeave += gcnew System::EventHandler(this, &MyForm::label64_MouseLeave);
			   // 
			   // listBox10
			   // 
			   this->listBox10->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox10->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox10->ForeColor = System::Drawing::Color::White;
			   this->listBox10->FormattingEnabled = true;
			   this->listBox10->ItemHeight = 25;
			   this->listBox10->Location = System::Drawing::Point(588, 125);
			   this->listBox10->Margin = System::Windows::Forms::Padding(2);
			   this->listBox10->Name = L"listBox10";
			   this->listBox10->Size = System::Drawing::Size(488, 279);
			   this->listBox10->TabIndex = 47;
			   // 
			   // panel22
			   // 
			   this->panel22->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)),
				   static_cast<System::Int32>(static_cast<System::Byte>(40)));
			   this->panel22->Location = System::Drawing::Point(6, 58);
			   this->panel22->Name = L"panel22";
			   this->panel22->Size = System::Drawing::Size(1081, 373);
			   this->panel22->TabIndex = 52;
			   // 
			   // panelDisco
			   // 
			   this->panelDisco->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panelDisco->Controls->Add(this->label103);
			   this->panelDisco->Controls->Add(this->label102);
			   this->panelDisco->Controls->Add(this->listBox11);
			   this->panelDisco->Controls->Add(this->listBox12);
			   this->panelDisco->Controls->Add(this->panel23);
			   this->panelDisco->ForeColor = System::Drawing::Color::White;
			   this->panelDisco->Location = System::Drawing::Point(288, 77);
			   this->panelDisco->Margin = System::Windows::Forms::Padding(2);
			   this->panelDisco->Name = L"panelDisco";
			   this->panelDisco->Size = System::Drawing::Size(1096, 519);
			   this->panelDisco->TabIndex = 56;
			   // 
			   // label103
			   // 
			   this->label103->AutoSize = true;
			   this->label103->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label103->ForeColor = System::Drawing::Color::White;
			   this->label103->Location = System::Drawing::Point(610, 41);
			   this->label103->Name = L"label103";
			   this->label103->Size = System::Drawing::Size(130, 32);
			   this->label103->TabIndex = 67;
			   this->label103->Text = L"Canciones";
			   // 
			   // label102
			   // 
			   this->label102->AutoSize = true;
			   this->label102->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label102->ForeColor = System::Drawing::Color::White;
			   this->label102->Location = System::Drawing::Point(17, 39);
			   this->label102->Name = L"label102";
			   this->label102->Size = System::Drawing::Size(206, 32);
			   this->label102->TabIndex = 66;
			   this->label102->Text = L"Albumes/Singles";
			   // 
			   // listBox11
			   // 
			   this->listBox11->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox11->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox11->ForeColor = System::Drawing::Color::White;
			   this->listBox11->FormattingEnabled = true;
			   this->listBox11->ItemHeight = 25;
			   this->listBox11->Location = System::Drawing::Point(16, 89);
			   this->listBox11->Margin = System::Windows::Forms::Padding(2);
			   this->listBox11->Name = L"listBox11";
			   this->listBox11->Size = System::Drawing::Size(475, 354);
			   this->listBox11->TabIndex = 48;
			   // 
			   // listBox12
			   // 
			   this->listBox12->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox12->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox12->ForeColor = System::Drawing::Color::White;
			   this->listBox12->FormattingEnabled = true;
			   this->listBox12->ItemHeight = 25;
			   this->listBox12->Location = System::Drawing::Point(601, 89);
			   this->listBox12->Margin = System::Windows::Forms::Padding(2);
			   this->listBox12->Name = L"listBox12";
			   this->listBox12->Size = System::Drawing::Size(475, 354);
			   this->listBox12->TabIndex = 47;
			   // 
			   // panel23
			   // 
			   this->panel23->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)),
				   static_cast<System::Int32>(static_cast<System::Byte>(40)));
			   this->panel23->Location = System::Drawing::Point(4, 20);
			   this->panel23->Name = L"panel23";
			   this->panel23->Size = System::Drawing::Size(1086, 488);
			   this->panel23->TabIndex = 68;
			   // 
			   // PanelEditar
			   // 
			   this->PanelEditar->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->PanelEditar->Controls->Add(this->label74);
			   this->PanelEditar->Controls->Add(this->comboBox4);
			   this->PanelEditar->Controls->Add(this->label87);
			   this->PanelEditar->Controls->Add(this->textBox28);
			   this->PanelEditar->Controls->Add(this->label88);
			   this->PanelEditar->Controls->Add(this->comboBox5);
			   this->PanelEditar->Controls->Add(this->label89);
			   this->PanelEditar->Controls->Add(this->textBox17);
			   this->PanelEditar->Controls->Add(this->label63);
			   this->PanelEditar->Controls->Add(this->panel20);
			   this->PanelEditar->Controls->Add(this->panel21);
			   this->PanelEditar->ForeColor = System::Drawing::Color::White;
			   this->PanelEditar->Location = System::Drawing::Point(288, 77);
			   this->PanelEditar->Margin = System::Windows::Forms::Padding(2);
			   this->PanelEditar->Name = L"PanelEditar";
			   this->PanelEditar->Size = System::Drawing::Size(1096, 519);
			   this->PanelEditar->TabIndex = 54;
			   // 
			   // label74
			   // 
			   this->label74->AutoSize = true;
			   this->label74->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label74->ForeColor = System::Drawing::Color::White;
			   this->label74->Location = System::Drawing::Point(6, 5);
			   this->label74->Name = L"label74";
			   this->label74->Size = System::Drawing::Size(204, 32);
			   this->label74->TabIndex = 66;
			   this->label74->Text = L"Editar Canciones";
			   // 
			   // comboBox4
			   // 
			   this->comboBox4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->comboBox4->FormattingEnabled = true;
			   this->comboBox4->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
				   L"playlist", L"recomendado", L"favorito", L"infantil",
					   L"instrumental"
			   });
			   this->comboBox4->Location = System::Drawing::Point(169, 89);
			   this->comboBox4->Margin = System::Windows::Forms::Padding(2);
			   this->comboBox4->Name = L"comboBox4";
			   this->comboBox4->Size = System::Drawing::Size(163, 21);
			   this->comboBox4->TabIndex = 75;
			   // 
			   // label87
			   // 
			   this->label87->AutoSize = true;
			   this->label87->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label87->ForeColor = System::Drawing::Color::White;
			   this->label87->Location = System::Drawing::Point(165, 65);
			   this->label87->Name = L"label87";
			   this->label87->Size = System::Drawing::Size(79, 19);
			   this->label87->TabIndex = 74;
			   this->label87->Text = L"Categoria:";
			   // 
			   // textBox28
			   // 
			   this->textBox28->BackColor = System::Drawing::Color::DimGray;
			   this->textBox28->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox28->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox28->Location = System::Drawing::Point(9, 238);
			   this->textBox28->Margin = System::Windows::Forms::Padding(2);
			   this->textBox28->Multiline = true;
			   this->textBox28->Name = L"textBox28";
			   this->textBox28->Size = System::Drawing::Size(323, 39);
			   this->textBox28->TabIndex = 73;
			   // 
			   // label88
			   // 
			   this->label88->AutoSize = true;
			   this->label88->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label88->ForeColor = System::Drawing::Color::White;
			   this->label88->Location = System::Drawing::Point(6, 211);
			   this->label88->Name = L"label88";
			   this->label88->Size = System::Drawing::Size(87, 19);
			   this->label88->TabIndex = 72;
			   this->label88->Text = L"Descripcion";
			   // 
			   // comboBox5
			   // 
			   this->comboBox5->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			   this->comboBox5->FormattingEnabled = true;
			   this->comboBox5->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
				   L"pop", L"Corridos", L"cristianos", L"electrónica",
					   L"reguetón", L"rock", L"clasicas"
			   });
			   this->comboBox5->Location = System::Drawing::Point(9, 89);
			   this->comboBox5->Margin = System::Windows::Forms::Padding(2);
			   this->comboBox5->Name = L"comboBox5";
			   this->comboBox5->Size = System::Drawing::Size(148, 21);
			   this->comboBox5->TabIndex = 71;
			   // 
			   // label89
			   // 
			   this->label89->AutoSize = true;
			   this->label89->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label89->ForeColor = System::Drawing::Color::White;
			   this->label89->Location = System::Drawing::Point(5, 65);
			   this->label89->Name = L"label89";
			   this->label89->Size = System::Drawing::Size(152, 19);
			   this->label89->TabIndex = 70;
			   this->label89->Text = L"Genero de la Cancion";
			   // 
			   // textBox17
			   // 
			   this->textBox17->BackColor = System::Drawing::Color::DimGray;
			   this->textBox17->BorderStyle = System::Windows::Forms::BorderStyle::None;
			   this->textBox17->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->textBox17->Location = System::Drawing::Point(9, 159);
			   this->textBox17->Margin = System::Windows::Forms::Padding(2);
			   this->textBox17->Multiline = true;
			   this->textBox17->Name = L"textBox17";
			   this->textBox17->Size = System::Drawing::Size(323, 36);
			   this->textBox17->TabIndex = 48;
			   // 
			   // label63
			   // 
			   this->label63->AutoSize = true;
			   this->label63->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label63->ForeColor = System::Drawing::Color::White;
			   this->label63->Location = System::Drawing::Point(5, 133);
			   this->label63->Name = L"label63";
			   this->label63->Size = System::Drawing::Size(159, 19);
			   this->label63->TabIndex = 41;
			   this->label63->Text = L"Nombre de la Cancion";
			   // 
			   // panel20
			   // 
			   this->panel20->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(36)), static_cast<System::Int32>(static_cast<System::Byte>(36)),
				   static_cast<System::Int32>(static_cast<System::Byte>(36)));
			   this->panel20->Controls->Add(this->label91);
			   this->panel20->Controls->Add(this->listBox8);
			   this->panel20->Controls->Add(this->label90);
			   this->panel20->Controls->Add(this->listBox7);
			   this->panel20->Location = System::Drawing::Point(347, 5);
			   this->panel20->Name = L"panel20";
			   this->panel20->Size = System::Drawing::Size(743, 510);
			   this->panel20->TabIndex = 78;
			   // 
			   // label91
			   // 
			   this->label91->AutoSize = true;
			   this->label91->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label91->ForeColor = System::Drawing::Color::White;
			   this->label91->Location = System::Drawing::Point(396, 90);
			   this->label91->Name = L"label91";
			   this->label91->Size = System::Drawing::Size(130, 32);
			   this->label91->TabIndex = 77;
			   this->label91->Text = L"Canciones";
			   // 
			   // listBox8
			   // 
			   this->listBox8->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox8->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox8->ForeColor = System::Drawing::Color::White;
			   this->listBox8->FormattingEnabled = true;
			   this->listBox8->ItemHeight = 25;
			   this->listBox8->Location = System::Drawing::Point(23, 132);
			   this->listBox8->Margin = System::Windows::Forms::Padding(2);
			   this->listBox8->Name = L"listBox8";
			   this->listBox8->Size = System::Drawing::Size(347, 279);
			   this->listBox8->TabIndex = 54;
			   // 
			   // label90
			   // 
			   this->label90->AutoSize = true;
			   this->label90->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label90->ForeColor = System::Drawing::Color::White;
			   this->label90->Location = System::Drawing::Point(24, 86);
			   this->label90->Name = L"label90";
			   this->label90->Size = System::Drawing::Size(206, 32);
			   this->label90->TabIndex = 76;
			   this->label90->Text = L"Albumes/Singles";
			   // 
			   // listBox7
			   // 
			   this->listBox7->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(49)),
				   static_cast<System::Int32>(static_cast<System::Byte>(54)));
			   this->listBox7->Font = (gcnew System::Drawing::Font(L"Segoe UI", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->listBox7->ForeColor = System::Drawing::Color::White;
			   this->listBox7->FormattingEnabled = true;
			   this->listBox7->ItemHeight = 25;
			   this->listBox7->Location = System::Drawing::Point(389, 134);
			   this->listBox7->Margin = System::Windows::Forms::Padding(2);
			   this->listBox7->Name = L"listBox7";
			   this->listBox7->Size = System::Drawing::Size(347, 279);
			   this->listBox7->TabIndex = 47;
			   // 
			   // panel21
			   // 
			   this->panel21->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(50)), static_cast<System::Int32>(static_cast<System::Byte>(50)),
				   static_cast<System::Int32>(static_cast<System::Byte>(50)));
			   this->panel21->Controls->Add(this->label98);
			   this->panel21->Controls->Add(this->label61);
			   this->panel21->Controls->Add(this->pictureBox60);
			   this->panel21->Controls->Add(this->label62);
			   this->panel21->Location = System::Drawing::Point(6, 298);
			   this->panel21->Name = L"panel21";
			   this->panel21->Size = System::Drawing::Size(326, 216);
			   this->panel21->TabIndex = 79;
			   // 
			   // label98
			   // 
			   this->label98->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label98->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label98->ForeColor = System::Drawing::Color::White;
			   this->label98->Location = System::Drawing::Point(6, 159);
			   this->label98->Name = L"label98";
			   this->label98->Size = System::Drawing::Size(138, 51);
			   this->label98->TabIndex = 79;
			   this->label98->Text = L"Seleccionar Imagen";
			   this->label98->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // label61
			   // 
			   this->label61->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label61->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label61->ForeColor = System::Drawing::Color::White;
			   this->label61->Location = System::Drawing::Point(185, 160);
			   this->label61->Name = L"label61";
			   this->label61->Size = System::Drawing::Size(138, 51);
			   this->label61->TabIndex = 44;
			   this->label61->Text = L"Editar";
			   this->label61->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label61->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label61_MouseClick_1);
			   this->label61->MouseEnter += gcnew System::EventHandler(this, &MyForm::label61_MouseEnter);
			   this->label61->MouseLeave += gcnew System::EventHandler(this, &MyForm::label61_MouseLeave);
			   // 
			   // pictureBox60
			   // 
			   this->pictureBox60->BackColor = System::Drawing::Color::Transparent;
			   this->pictureBox60->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox60.Image")));
			   this->pictureBox60->Location = System::Drawing::Point(80, 41);
			   this->pictureBox60->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox60->Name = L"pictureBox60";
			   this->pictureBox60->Size = System::Drawing::Size(153, 109);
			   this->pictureBox60->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox60->TabIndex = 45;
			   this->pictureBox60->TabStop = false;
			   // 
			   // label62
			   // 
			   this->label62->AutoSize = true;
			   this->label62->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label62->ForeColor = System::Drawing::Color::White;
			   this->label62->Location = System::Drawing::Point(121, 9);
			   this->label62->Name = L"label62";
			   this->label62->Size = System::Drawing::Size(67, 19);
			   this->label62->TabIndex = 50;
			   this->label62->Text = L"Portada ";
			   // 
			   // panel6
			   // 
			   this->panel6->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->panel6->Controls->Add(this->pictureBox50);
			   this->panel6->Controls->Add(this->label15);
			   this->panel6->Controls->Add(this->pictureBox4);
			   this->panel6->Controls->Add(this->label51);
			   this->panel6->Controls->Add(this->pictureBox5);
			   this->panel6->Controls->Add(this->label52);
			   this->panel6->Controls->Add(this->pictureBox45);
			   this->panel6->Controls->Add(this->pictureBox46);
			   this->panel6->Controls->Add(this->pictureBox47);
			   this->panel6->Controls->Add(this->pictureBox48);
			   this->panel6->Controls->Add(this->pictureBox49);
			   this->panel6->Controls->Add(this->label53);
			   this->panel6->Controls->Add(this->label54);
			   this->panel6->Controls->Add(this->label55);
			   this->panel6->Controls->Add(this->label56);
			   this->panel6->Controls->Add(this->panel7);
			   this->panel6->Location = System::Drawing::Point(17, 11);
			   this->panel6->Margin = System::Windows::Forms::Padding(2);
			   this->panel6->Name = L"panel6";
			   this->panel6->Size = System::Drawing::Size(261, 587);
			   this->panel6->TabIndex = 38;
			   // 
			   // pictureBox50
			   // 
			   this->pictureBox50->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox50->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox50.Image")));
			   this->pictureBox50->Location = System::Drawing::Point(15, 520);
			   this->pictureBox50->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox50->Name = L"pictureBox50";
			   this->pictureBox50->Size = System::Drawing::Size(46, 37);
			   this->pictureBox50->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox50->TabIndex = 32;
			   this->pictureBox50->TabStop = false;
			   // 
			   // label15
			   // 
			   this->label15->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label15->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label15->ForeColor = System::Drawing::Color::White;
			   this->label15->Location = System::Drawing::Point(67, 168);
			   this->label15->Name = L"label15";
			   this->label15->Size = System::Drawing::Size(179, 39);
			   this->label15->TabIndex = 31;
			   this->label15->Text = L"Subir Album";
			   this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label15->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label15_MouseClick);
			   this->label15->MouseEnter += gcnew System::EventHandler(this, &MyForm::label15_MouseEnter);
			   this->label15->MouseLeave += gcnew System::EventHandler(this, &MyForm::label15_MouseLeave);
			   // 
			   // pictureBox4
			   // 
			   this->pictureBox4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox4->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox4.Image")));
			   this->pictureBox4->Location = System::Drawing::Point(16, 168);
			   this->pictureBox4->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox4->Name = L"pictureBox4";
			   this->pictureBox4->Size = System::Drawing::Size(46, 39);
			   this->pictureBox4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox4->TabIndex = 30;
			   this->pictureBox4->TabStop = false;
			   // 
			   // label51
			   // 
			   this->label51->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label51->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label51->ForeColor = System::Drawing::Color::White;
			   this->label51->Location = System::Drawing::Point(67, 460);
			   this->label51->Name = L"label51";
			   this->label51->Size = System::Drawing::Size(180, 37);
			   this->label51->TabIndex = 29;
			   this->label51->Text = L"Configuracion";
			   this->label51->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label51->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label51_MouseClick);
			   this->label51->MouseEnter += gcnew System::EventHandler(this, &MyForm::label51_MouseEnter);
			   this->label51->MouseLeave += gcnew System::EventHandler(this, &MyForm::label51_MouseLeave);
			   // 
			   // pictureBox5
			   // 
			   this->pictureBox5->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox5->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox5.Image")));
			   this->pictureBox5->Location = System::Drawing::Point(18, 20);
			   this->pictureBox5->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox5->Name = L"pictureBox5";
			   this->pictureBox5->Size = System::Drawing::Size(212, 103);
			   this->pictureBox5->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox5->TabIndex = 28;
			   this->pictureBox5->TabStop = false;
			   // 
			   // label52
			   // 
			   this->label52->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label52->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label52->ForeColor = System::Drawing::Color::White;
			   this->label52->Location = System::Drawing::Point(66, 230);
			   this->label52->Name = L"label52";
			   this->label52->Size = System::Drawing::Size(180, 42);
			   this->label52->TabIndex = 18;
			   this->label52->Text = L"Actualizar";
			   this->label52->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label52->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label52_MouseClick);
			   this->label52->MouseEnter += gcnew System::EventHandler(this, &MyForm::label52_MouseEnter);
			   this->label52->MouseLeave += gcnew System::EventHandler(this, &MyForm::label52_MouseLeave);
			   // 
			   // pictureBox45
			   // 
			   this->pictureBox45->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox45->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox45.Image")));
			   this->pictureBox45->Location = System::Drawing::Point(15, 459);
			   this->pictureBox45->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox45->Name = L"pictureBox45";
			   this->pictureBox45->Size = System::Drawing::Size(46, 37);
			   this->pictureBox45->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox45->TabIndex = 27;
			   this->pictureBox45->TabStop = false;
			   // 
			   // pictureBox46
			   // 
			   this->pictureBox46->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox46->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox46.Image")));
			   this->pictureBox46->Location = System::Drawing::Point(15, 402);
			   this->pictureBox46->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox46->Name = L"pictureBox46";
			   this->pictureBox46->Size = System::Drawing::Size(46, 37);
			   this->pictureBox46->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox46->TabIndex = 26;
			   this->pictureBox46->TabStop = false;
			   // 
			   // pictureBox47
			   // 
			   this->pictureBox47->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox47->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox47.Image")));
			   this->pictureBox47->Location = System::Drawing::Point(15, 344);
			   this->pictureBox47->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox47->Name = L"pictureBox47";
			   this->pictureBox47->Size = System::Drawing::Size(46, 37);
			   this->pictureBox47->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox47->TabIndex = 25;
			   this->pictureBox47->TabStop = false;
			   // 
			   // pictureBox48
			   // 
			   this->pictureBox48->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox48->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox48.Image")));
			   this->pictureBox48->Location = System::Drawing::Point(15, 288);
			   this->pictureBox48->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox48->Name = L"pictureBox48";
			   this->pictureBox48->Size = System::Drawing::Size(46, 37);
			   this->pictureBox48->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox48->TabIndex = 24;
			   this->pictureBox48->TabStop = false;
			   // 
			   // pictureBox49
			   // 
			   this->pictureBox49->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->pictureBox49->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox49.Image")));
			   this->pictureBox49->Location = System::Drawing::Point(15, 230);
			   this->pictureBox49->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox49->Name = L"pictureBox49";
			   this->pictureBox49->Size = System::Drawing::Size(46, 42);
			   this->pictureBox49->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox49->TabIndex = 23;
			   this->pictureBox49->TabStop = false;
			   // 
			   // label53
			   // 
			   this->label53->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label53->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label53->ForeColor = System::Drawing::Color::White;
			   this->label53->Location = System::Drawing::Point(66, 520);
			   this->label53->Name = L"label53";
			   this->label53->Size = System::Drawing::Size(180, 37);
			   this->label53->TabIndex = 22;
			   this->label53->Text = L"Salir";
			   this->label53->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label53->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label53_MouseClick);
			   this->label53->MouseEnter += gcnew System::EventHandler(this, &MyForm::label53_MouseEnter);
			   this->label53->MouseLeave += gcnew System::EventHandler(this, &MyForm::label53_MouseLeave);
			   // 
			   // label54
			   // 
			   this->label54->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label54->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label54->ForeColor = System::Drawing::Color::White;
			   this->label54->Location = System::Drawing::Point(66, 402);
			   this->label54->Name = L"label54";
			   this->label54->Size = System::Drawing::Size(180, 37);
			   this->label54->TabIndex = 21;
			   this->label54->Text = L"Estadisticas";
			   this->label54->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label54->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label54_MouseClick);
			   this->label54->MouseEnter += gcnew System::EventHandler(this, &MyForm::label54_MouseEnter);
			   this->label54->MouseLeave += gcnew System::EventHandler(this, &MyForm::label54_MouseLeave);
			   // 
			   // label55
			   // 
			   this->label55->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label55->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label55->ForeColor = System::Drawing::Color::White;
			   this->label55->Location = System::Drawing::Point(66, 344);
			   this->label55->Name = L"label55";
			   this->label55->Size = System::Drawing::Size(180, 37);
			   this->label55->TabIndex = 20;
			   this->label55->Text = L"Discografia";
			   this->label55->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label55->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label55_MouseClick);
			   this->label55->MouseEnter += gcnew System::EventHandler(this, &MyForm::label55_MouseEnter);
			   this->label55->MouseLeave += gcnew System::EventHandler(this, &MyForm::label55_MouseLeave);
			   // 
			   // label56
			   // 
			   this->label56->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				   static_cast<System::Int32>(static_cast<System::Byte>(64)));
			   this->label56->Font = (gcnew System::Drawing::Font(L"Segoe UI", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label56->ForeColor = System::Drawing::Color::White;
			   this->label56->Location = System::Drawing::Point(66, 288);
			   this->label56->Name = L"label56";
			   this->label56->Size = System::Drawing::Size(180, 37);
			   this->label56->TabIndex = 19;
			   this->label56->Text = L"Eliminar";
			   this->label56->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label56->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::label56_MouseClick);
			   this->label56->MouseEnter += gcnew System::EventHandler(this, &MyForm::label56_MouseEnter);
			   this->label56->MouseLeave += gcnew System::EventHandler(this, &MyForm::label56_MouseLeave);
			   // 
			   // panel7
			   // 
			   this->panel7->BackColor = System::Drawing::Color::Gray;
			   this->panel7->ForeColor = System::Drawing::Color::Transparent;
			   this->panel7->Location = System::Drawing::Point(18, 134);
			   this->panel7->Margin = System::Windows::Forms::Padding(2);
			   this->panel7->Name = L"panel7";
			   this->panel7->Size = System::Drawing::Size(212, 10);
			   this->panel7->TabIndex = 17;
			   // 
			   // panel8
			   // 
			   this->panel8->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->panel8->Controls->Add(this->pictureBox56);
			   this->panel8->Controls->Add(this->label75);
			   this->panel8->Controls->Add(this->progressBar2);
			   this->panel8->Controls->Add(this->pictureBox54);
			   this->panel8->Controls->Add(this->pictureBox52);
			   this->panel8->Controls->Add(this->label93);
			   this->panel8->Controls->Add(this->pictureBox51);
			   this->panel8->Controls->Add(this->pictureBox53);
			   this->panel8->Controls->Add(this->label57);
			   this->panel8->Location = System::Drawing::Point(17, 608);
			   this->panel8->Margin = System::Windows::Forms::Padding(2);
			   this->panel8->Name = L"panel8";
			   this->panel8->Size = System::Drawing::Size(1367, 115);
			   this->panel8->TabIndex = 39;
			   // 
			   // pictureBox56
			   // 
			   this->pictureBox56->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->pictureBox56->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox56.Image")));
			   this->pictureBox56->Location = System::Drawing::Point(636, 14);
			   this->pictureBox56->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox56->Name = L"pictureBox56";
			   this->pictureBox56->Size = System::Drawing::Size(64, 52);
			   this->pictureBox56->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox56->TabIndex = 40;
			   this->pictureBox56->TabStop = false;
			   this->pictureBox56->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox56_MouseClick);
			   // 
			   // label75
			   // 
			   this->label75->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->label75->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label75->ForeColor = System::Drawing::Color::White;
			   this->label75->Location = System::Drawing::Point(900, 70);
			   this->label75->Name = L"label75";
			   this->label75->Size = System::Drawing::Size(58, 32);
			   this->label75->TabIndex = 38;
			   this->label75->Text = L"0:00";
			   this->label75->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   // 
			   // progressBar2
			   // 
			   this->progressBar2->Location = System::Drawing::Point(504, 82);
			   this->progressBar2->Name = L"progressBar2";
			   this->progressBar2->Size = System::Drawing::Size(390, 10);
			   this->progressBar2->TabIndex = 37;
			   // 
			   // pictureBox54
			   // 
			   this->pictureBox54->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->pictureBox54->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox54.Image")));
			   this->pictureBox54->Location = System::Drawing::Point(9, 20);
			   this->pictureBox54->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox54->Name = L"pictureBox54";
			   this->pictureBox54->Size = System::Drawing::Size(109, 78);
			   this->pictureBox54->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox54->TabIndex = 33;
			   this->pictureBox54->TabStop = false;
			   // 
			   // pictureBox52
			   // 
			   this->pictureBox52->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->pictureBox52->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox52.Image")));
			   this->pictureBox52->Location = System::Drawing::Point(581, 20);
			   this->pictureBox52->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox52->Name = L"pictureBox52";
			   this->pictureBox52->Size = System::Drawing::Size(50, 42);
			   this->pictureBox52->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox52->TabIndex = 36;
			   this->pictureBox52->TabStop = false;
			   this->pictureBox52->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox52_MouseClick);
			   // 
			   // label93
			   // 
			   this->label93->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->label93->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label93->ForeColor = System::Drawing::Color::White;
			   this->label93->Location = System::Drawing::Point(124, 72);
			   this->label93->Name = L"label93";
			   this->label93->Size = System::Drawing::Size(344, 28);
			   this->label93->TabIndex = 35;
			   this->label93->Text = L"Artista";
			   this->label93->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			   // 
			   // pictureBox51
			   // 
			   this->pictureBox51->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->pictureBox51->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox51.Image")));
			   this->pictureBox51->Location = System::Drawing::Point(772, 20);
			   this->pictureBox51->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox51->Name = L"pictureBox51";
			   this->pictureBox51->Size = System::Drawing::Size(48, 42);
			   this->pictureBox51->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox51->TabIndex = 34;
			   this->pictureBox51->TabStop = false;
			   this->pictureBox51->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox51_MouseClick);
			   // 
			   // pictureBox53
			   // 
			   this->pictureBox53->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->pictureBox53->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox53.Image")));
			   this->pictureBox53->Location = System::Drawing::Point(704, 14);
			   this->pictureBox53->Margin = System::Windows::Forms::Padding(2);
			   this->pictureBox53->Name = L"pictureBox53";
			   this->pictureBox53->Size = System::Drawing::Size(64, 52);
			   this->pictureBox53->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			   this->pictureBox53->TabIndex = 32;
			   this->pictureBox53->TabStop = false;
			   this->pictureBox53->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox53_MouseClick);
			   // 
			   // label57
			   // 
			   this->label57->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			   this->label57->Font = (gcnew System::Drawing::Font(L"Segoe UI", 7.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label57->ForeColor = System::Drawing::Color::White;
			   this->label57->Location = System::Drawing::Point(123, 20);
			   this->label57->Name = L"label57";
			   this->label57->Size = System::Drawing::Size(345, 46);
			   this->label57->TabIndex = 28;
			   this->label57->Text = L"Nombre Rola";
			   this->label57->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			   // 
			   // pictureBox57
			   // 
			   this->pictureBox57->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox57.Image")));
			   this->pictureBox57->Location = System::Drawing::Point(1153, 13);
			   this->pictureBox57->Name = L"pictureBox57";
			   this->pictureBox57->Size = System::Drawing::Size(60, 54);
			   this->pictureBox57->TabIndex = 36;
			   this->pictureBox57->TabStop = false;
			   // 
			   // label59
			   // 
			   this->label59->AutoSize = true;
			   this->label59->Font = (gcnew System::Drawing::Font(L"Segoe UI", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label59->ForeColor = System::Drawing::Color::White;
			   this->label59->Location = System::Drawing::Point(1234, 17);
			   this->label59->Name = L"label59";
			   this->label59->Size = System::Drawing::Size(135, 45);
			   this->label59->TabIndex = 35;
			   this->label59->Text = L"TuneUp";
			   // 
			   // label16
			   // 
			   this->label16->AutoSize = true;
			   this->label16->Font = (gcnew System::Drawing::Font(L"Segoe UI", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label16->ForeColor = System::Drawing::Color::White;
			   this->label16->Location = System::Drawing::Point(282, 32);
			   this->label16->Name = L"label16";
			   this->label16->Size = System::Drawing::Size(262, 32);
			   this->label16->TabIndex = 41;
			   this->label16->Text = L"Bienvenido de nuevo,";
			   // 
			   // MyForm
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(1395, 734);
			   this->Controls->Add(this->panelArtista);
			   this->Controls->Add(this->PanelRegistroUsuario);
			   this->Controls->Add(this->PanelRegistroArtista);
			   this->Controls->Add(this->panel1);
			   this->Controls->Add(this->PanelUsuario);
			   this->Name = L"MyForm";
			   this->Text = L"MyForm";
			   this->panel1->ResumeLayout(false);
			   this->panel1->PerformLayout();
			   this->panel2->ResumeLayout(false);
			   this->panel2->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			   this->PanelRegistroArtista->ResumeLayout(false);
			   this->PanelRegistroArtista->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox26))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox9))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox12))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox16))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox17))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox15))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox18))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox14))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox13))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox11))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox10))->EndInit();
			   this->panel18->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
			   this->PanelRegistroUsuario->ResumeLayout(false);
			   this->PanelRegistroUsuario->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox21))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox24))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox25))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox28))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox20))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox22))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox23))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox19))->EndInit();
			   this->panel19->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->EndInit();
			   this->PanelUsuario->ResumeLayout(false);
			   this->PanelUsuario->PerformLayout();
			   this->PanelCatalogo->ResumeLayout(false);
			   this->PanelCatalogo->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox33))->EndInit();
			   this->Panelstatsusu->ResumeLayout(false);
			   this->Panelstatsusu->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView4))->EndInit();
			   this->PanelConfig->ResumeLayout(false);
			   this->PanelConfig->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox44))->EndInit();
			   this->PanelCrearPlay->ResumeLayout(false);
			   this->PanelCrearPlay->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox41))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox40))->EndInit();
			   this->PanelFav->ResumeLayout(false);
			   this->PanelFav->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox43))->EndInit();
			   this->PanelVerPlay->ResumeLayout(false);
			   this->PanelVerPlay->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox39))->EndInit();
			   this->panel14->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->axWindowsMediaPlayer2))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox38))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox37))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox36))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox35))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox34))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox32))->EndInit();
			   this->panel11->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox64))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox42))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox31))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox30))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox29))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox27))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox8))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox6))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox7))->EndInit();
			   this->panelArtista->ResumeLayout(false);
			   this->panelArtista->PerformLayout();
			   this->PanelConfi->ResumeLayout(false);
			   this->PanelConfi->PerformLayout();
			   this->panel13->ResumeLayout(false);
			   this->panel13->PerformLayout();
			   this->panel10->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox63))->EndInit();
			   this->panelStats->ResumeLayout(false);
			   this->panelStats->PerformLayout();
			   this->panel24->ResumeLayout(false);
			   this->panel24->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView2))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			   this->PanelSubir->ResumeLayout(false);
			   this->PanelSubir->PerformLayout();
			   this->panel16->ResumeLayout(false);
			   this->panel16->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox58))->EndInit();
			   this->PanelEliminar->ResumeLayout(false);
			   this->PanelEliminar->PerformLayout();
			   this->panelDisco->ResumeLayout(false);
			   this->panelDisco->PerformLayout();
			   this->PanelEditar->ResumeLayout(false);
			   this->PanelEditar->PerformLayout();
			   this->panel20->ResumeLayout(false);
			   this->panel20->PerformLayout();
			   this->panel21->ResumeLayout(false);
			   this->panel21->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox60))->EndInit();
			   this->panel6->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox50))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox5))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox45))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox46))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox47))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox48))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox49))->EndInit();
			   this->panel8->ResumeLayout(false);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox56))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox54))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox52))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox51))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox53))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox57))->EndInit();
			   this->ResumeLayout(false);

		   }
#pragma endregion


		   // Parte estetica Menu Inicial 
		   //=====================================================================================// 
	private: System::Void textBox1_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		if (textBox1->Text == "Nombre de Usuario") {
			textBox1->Text = "";
		}
	}

	private: System::Void textBox1_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		if (textBox1->Text == "" || textBox1->Text == " ") {
			textBox1->Text = "Nombre de Usuario";
			textBox1->Font = gcnew System::Drawing::Font("Segoe UI", 16, FontStyle::Regular);
		}
	}

	private: System::Void textBox2_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		if (textBox2->Text == "Contraseña") {
			textBox2->Text = "";
		}
	}

	private: System::Void textBox2_MouseLeave_1(System::Object^ sender, System::EventArgs^ e) {
		if (textBox2->Text == "" || textBox2->Text == " ") {
			textBox2->Text = "Contraseña";
			textBox2->Font = gcnew System::Drawing::Font("Segoe UI", 16, FontStyle::Regular);
		}
	}

	private: System::Void linkLabel1_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		linkLabel1->Cursor = Cursors::Hand;
	}

	private: System::Void linkLabel1_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
		panel2->Visible = true;
	}

		   // Fin parte Estetica Menu 
		   //=====================================================================================// 

		   // Iniciar pagina de Registro como usuario o artista 
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
		bool ch1 = false;
		bool ch2 = false;
		if (checkBox1->Checked && !checkBox2->Checked) {
			ch1 = true;
			panel2->Visible = false;
			IdboxArt->Text = generar().ToString();

		}
		else if (checkBox2->Checked && !checkBox1->Checked) {
			ch2 = true;
			panel2->Visible = false;
			idboxusu->Text = generar().ToString();
		}
		else if (!ch1 && !ch2) {
			MessageBox::Show("No puede seleccionar ambas a la vez", "Error");
		}

		if (ch1) {
			PanelRegistroArtista->Visible = true;
			panel1->Visible = (false);
			panel2->Visible = false;
			ch1 = false;
		}
		else if (ch2) {
			PanelRegistroUsuario->Visible = true;
			panel1->Visible = (false);
			panel2->Visible = false;
			ch2 = false;
		}
		ch1 = false;
		ch2 = false;
	}

		   // Selecionar una imagen en la parte de registro Usuarios 
		   //=====================================================================================// 
	private: System::Void button5_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Imágenes (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
		ofd->Title = "Seleccione una imagen";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				System::Drawing::Image^ img = System::Drawing::Image::FromFile(ofd->FileName);
				pictureBox2->Image = img;
				pictureBox2->SizeMode = PictureBoxSizeMode::Zoom;
			}
			catch (System::Exception^ ex) {
				MessageBox::Show("Error al cargar la imagen:\n" + ex->Message, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}

		   // Registro artistas 
		   // Registros Generales 
		   //=====================================================================================// 
	private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) {
		bool valido = true;

		if (String::IsNullOrWhiteSpace(UsuarioBox->Text)) {
			MessageBox::Show("El usuario es obligatorio", "Error");
			valido = false;
		}
		else if (ListaUsuarios->Buscar(nullptr, UsuarioBox->Text) != nullptr || ListaArtistas->Buscar(nullptr, UsuarioBox->Text)) {
			MessageBox::Show("El Nombre de usuario ya esta en uso", "Error");
			valido = false;
		}

		String^ email = CorreoBox->Text->Trim();
		String^ patronEmail = "^([\\w\\.\\-]+)@([\\w\\-]+\\.)+([A-Za-z]{2,4})$";
		if (String::IsNullOrWhiteSpace(email) ||
			!Regex::IsMatch(email, patronEmail)) {
			MessageBox::Show("Correo Invalido", "Error");
			valido = false;
		}

		if (String::IsNullOrWhiteSpace(NomArtistico->Text)) {
			MessageBox::Show("El Nombre Artistico es obligatorio", "Error");
			valido = false;
		}
		if (ListaArtistas->Buscar(nullptr, NomArtistico->Text) != nullptr) {
			MessageBox::Show("El Nombre artistico ya existe", "Error");
			valido = false;
		}

		if (String::IsNullOrWhiteSpace(Nombre->Text)) {
			MessageBox::Show("El Nombre es obligatorio", "Error");
			valido = false;
		}
		if (pictureBox2->Image == x) {
			MessageBox::Show("Debe seleccionar una imagen", "Error");
			valido = false;
		}

		String^ pass = contra->Text;
		if (pass->Length < 6) {
			MessageBox::Show("Minimo 6 caracteres", "Error");
			valido = false;
		}

		if (pass != valicontra->Text) {
			MessageBox::Show("Las contraseñas no coinciden", "Error");
			valido = false;
		}

		if (Fecha->Value.Date > DateTime::Today) {
			MessageBox::Show("La fecha no puede ser en el futuro", "Error");
			valido = false;
		}

		if (PaisBox->SelectedIndex < 0) {
			MessageBox::Show("El pais es obligatorio", "Error");
			valido = false;
		}

		if (Genero->SelectedIndex < 0) {
			MessageBox::Show("El Genero es obligatorio", "Error");
			valido = false;
		}
		if (!valido) return;

		Artistas^ u = gcnew Artistas(Int32::Parse(IdboxArt->Text->Trim()),
			UsuarioBox->Text->Trim(),
			CorreoBox->Text->Trim(),
			NomArtistico->Text->Trim(),
			Nombre->Text->Trim(),
			pictureBox2->Image,
			contra->Text,
			Fecha->Value,
			PaisBox->SelectedItem->ToString(),
			Genero->SelectedItem->ToString(), true
		);

		ListaArtistas->Agregar(u);
		MessageBox::Show("Todos los datos son validos. Procediendo al Login.", "Exito", MessageBoxButtons::OK, MessageBoxIcon::Information);
		UsuarioBox->Clear();
		CorreoBox->Clear();
		NomArtistico->Clear();
		Nombre->Clear();
		pictureBox2->Image = x;
		contra->Clear();
		valicontra->Clear();
		Fecha->Value = DateTime::Today;
		PaisBox->SelectedIndex = -1;
		Genero->SelectedIndex = -1;
		PanelRegistroArtista->Visible = false;
		panel1->Visible = true;
	}
		   // Regresar de la parte de registro, artista 
	private: System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) {
		PanelRegistroArtista->Visible = false;
		panel1->Visible = (true);
		panel2->Visible = false;
		checkBox2->Checked = false;
		checkBox1->Checked = false;
		UsuarioBox->Clear();
		CorreoBox->Clear();
		NomArtistico->Clear();
		Nombre->Clear();
		pictureBox2->Image = x;
		contra->Clear();
		valicontra->Clear();
		Fecha->Value = DateTime::Today;
		PaisBox->SelectedIndex = -1;
		Genero->SelectedIndex = -1;
		PanelRegistroArtista->Visible = false;
		panel1->Visible = true;
	}
		   // Registro Usuario 
	private: System::Void button8_Click(System::Object^ sender, System::EventArgs^ e) {
		bool valido = true;

		if (String::IsNullOrWhiteSpace(Usuario2->Text)) {
			MessageBox::Show("El usuario es obligatorio", "Error");
			valido = false;
		}
		else if (ListaUsuarios->Buscar(nullptr, Usuario2->Text) != nullptr || ListaArtistas->Buscar(nullptr, Usuario2->Text)) {
			MessageBox::Show("El Nombre de usuario ya esta en uso", "Error");
			valido = false;
		}

		if (String::IsNullOrWhiteSpace(Nombre2->Text)) {
			MessageBox::Show("El Nombre es obligatorio", "Error");
			valido = false;
		}

		if (pictureBox3->Image == x) {
			MessageBox::Show("Debe seleccionar una imagen", "Error");
			valido = false;
		}

		String^ pass = contra2->Text;
		if (pass->Length < 6) {
			MessageBox::Show("Minimo 6 caracteres", "Error");
			valido = false;
		}

		if (pass != valicontra2->Text) {
			MessageBox::Show("Las contraseñas no coinciden", "Error");
			valido = false;
		}

		String^ email = correou->Text->Trim();
		String^ patronEmail = "^([\\w\\.\\-]+)@([\\w\\-]+\\.)+([A-Za-z]{2,4})$";
		if (String::IsNullOrWhiteSpace(email) ||
			!Regex::IsMatch(email, patronEmail)) {
			MessageBox::Show("Correo Invalido", "Error");
			valido = false;
		}

		if (!valido) return;

		idboxusu->Text;


		Usuarios^ z = gcnew Usuarios(Int32::Parse(idboxusu->Text->Trim()),
			Usuario2->Text->Trim(),
			contra2->Text->Trim(),
			Nombre2->Text->Trim(),
			correou->Text->Trim(),
			edad2->Value,
			true,
			pictureBox3->Image);

		ListaUsuarios->Agregar(z);
		MessageBox::Show("Todos los datos son validos. Procediendo al Login.", "Exito", MessageBoxButtons::OK, MessageBoxIcon::Information);
		correou->Clear();
		Usuario2->Clear();
		Nombre2->Clear();
		pictureBox3->Image = x;
		contra2->Clear();
		valicontra2->Clear();
		edad2->Value = DateTime::Today;
		PanelRegistroUsuario->Visible = false;
		panel1->Visible = true;
	}
		   // Regresar del registro del usuario 
	private: System::Void button7_Click(System::Object^ sender, System::EventArgs^ e) {
		PanelRegistroUsuario->Visible = false;
		panel1->Visible = (true);
		panel2->Visible = false;
		checkBox2->Checked = false;
		checkBox1->Checked = false;
		Usuario2->Clear();
		Nombre2->Clear();
		pictureBox3->Image = x;
		contra2->Clear();
		valicontra2->Clear();
		edad2->Value = DateTime::Today;
		PanelRegistroUsuario->Visible = false;
		panel1->Visible = true;
	}
		   // Seleccionar Imagen de registro Usuario 
	private: System::Void button6_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Imágenes (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
		ofd->Title = "Seleccione una imagen";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				System::Drawing::Image^ img = System::Drawing::Image::FromFile(ofd->FileName);
				pictureBox3->Image = img;
				pictureBox3->SizeMode = PictureBoxSizeMode::Zoom;
			}
			catch (System::Exception^ ex) {
				MessageBox::Show("Error al cargar la imagen:\n" + ex->Message, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}
		   // Fin Registros 
		   //=====================================================================================// 


		   // Login 
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {

		Nodo<Usuarios^>^ resultadoU = ListaUsuarios->Buscar(nullptr, textBox1->Text);
		Nodo<Artistas^>^ resultadoA = ListaArtistas->Buscar(nullptr, textBox1->Text);

		if (resultadoA != nullptr) {
			if (textBox2->Text == resultadoA->data->Contrasena) {
				artistaActual = resultadoA->data;
				panel1->Visible = false;
				panelArtista->Visible = true;
				pictureBox5->Image = resultadoA->data->ImagenPerfil;
				label16->Text = "Bienvenido de nuevo, " + resultadoA->data->NombreArtistico;
			}
			else {
				MessageBox::Show("Contraseña incorrecta ", "Error");
			}
		}
		else if (resultadoU != nullptr) {
			if (textBox2->Text == resultadoU->data->getEncryptedPassword()) {
				panel1->Visible = false;
				PanelUsuario->Visible = true;
				pictureBox6->Image = resultadoU->data->getAvatarPath();
				label22->Text = "Bienvenido de nuevo, " + resultadoU->data->getUsername();
			}
			else {
				MessageBox::Show("Contraseña incorrecta ", "Error");
			}
		}
		else {
			MessageBox::Show("Usuario no encontrado", "Error");
		}
	}

		   // Log out en usuarios y artistas 
		   //=====================================================================================// 
	private: System::Void button15_Click(System::Object^ sender, System::EventArgs^ e) {
		PanelUsuario->Visible = false;
		panel1->Visible = true;
	}
	private: System::Void button12_Click(System::Object^ sender, System::EventArgs^ e) {
		panelArtista->Visible = false;
		panel1->Visible = true;
	}

	private: System::Void button31_Click(System::Object^ sender, System::EventArgs^ e) {
		panel2->Visible = false;
		checkBox1->Checked = false;
		checkBox2->Checked = false;
	}
		   // fin  log out 
		   //=====================================================================================// 


		   // Inicio estetica de botones 
		   //=====================================================================================// 
	private: System::Void label26_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label26->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox8->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label26_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label26->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox8->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label27_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label27->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox27->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label27_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label27->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox27->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label28_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label28->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox29->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label28_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label28->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox29->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label29_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label29->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox30->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label29_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label29->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox30->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label30_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label30->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox42->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label30_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label30->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox42->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void pictureBox33_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->pictureBox33->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void pictureBox33_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->pictureBox33->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label37_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label37->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label37_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label37->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
		   // Fin estetica botones inicio de sesion de Artista 
		   //=====================================================================================// 


		   // Controlar cuando aparecen los paneles super puestos entre si 
		   //=====================================================================================// 
	private: System::Void label26_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PanelFav->Visible = false;
		PanelCrearPlay->Visible = false;
		PanelVerPlay->Visible = false;
		PanelCatalogo->Visible = true;
		PanelConfig->Visible = false;
		Panelstatsusu->Visible = false;
	}
	private: System::Void label27_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PanelFav->Visible = false;
		PanelCrearPlay->Visible = false;
		PanelVerPlay->Visible = true;
		PanelCatalogo->Visible = false;
		PanelConfig->Visible = false;
		Panelstatsusu->Visible = false;
	}

	private: System::Void label28_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PanelFav->Visible = false;
		PanelCrearPlay->Visible = true;
		PanelVerPlay->Visible = false;
		PanelCatalogo->Visible = false;
		PanelConfig->Visible = false;
		Panelstatsusu->Visible = false;
	}
	private: System::Void label29_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PanelFav->Visible = true;
		PanelCrearPlay->Visible = false;
		PanelVerPlay->Visible = false;
		PanelCatalogo->Visible = false;
		Panelstatsusu->Visible = false;
	}
	private: System::Void label30_Click(System::Object^ sender, System::EventArgs^ e) {
		PanelUsuario->Visible = false;
		panel1->Visible = true;
	}

	private: System::Void label38_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label38->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox31->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label38_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label38->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox31->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label38_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PanelConfig->Visible = true;
		PanelFav->Visible = false;
		PanelCrearPlay->Visible = false;
		PanelVerPlay->Visible = false;
		PanelCatalogo->Visible = false;
		Panelstatsusu->Visible = false;
	}

	private: System::Void label15_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label15->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox4->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label15_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label15->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox4->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label52_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label52->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox49->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label52_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label52->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);;
		this->pictureBox49->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label56_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label56->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox48->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label56_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label56->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox48->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label55_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label55->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox47->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label55_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label55->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox47->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label54_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label54->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox46->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label54_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label54->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox46->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}

	private: System::Void label51_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label51->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox45->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label51_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label51->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox45->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label53_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label53->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox50->BackColor = System::Drawing::Color::PaleVioletRed;

	}
	private: System::Void label53_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label53->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox50->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label41_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label41->BackColor = System::Drawing::Color::PaleVioletRed;
	}

	private: System::Void label41_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label41->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label48_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label48->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label48_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label48->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label61_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label61->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label61_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label61->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label64_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label64->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label64_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label64->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label73_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label73->BackColor = System::Drawing::Color::PaleVioletRed;
		this->pictureBox64->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label73_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		this->label73->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
		this->pictureBox64->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label73_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		Panelstatsusu->Visible = true;
		PanelConfig->Visible = false;
		PanelFav->Visible = false;
		PanelCrearPlay->Visible = false;
		PanelVerPlay->Visible = false;
		PanelCatalogo->Visible = false;
	}
	private: System::Void label15_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		panelStats->Visible = false;
		PanelConfi->Visible = false;
		panelDisco->Visible = false;
		PanelEliminar->Visible = false;
		PanelEditar->Visible = false;
		PanelSubir->Visible = true;
	}
	private: System::Void label52_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		panelStats->Visible = false;
		PanelConfi->Visible = false;
		panelDisco->Visible = false;
		PanelEliminar->Visible = false;
		PanelEditar->Visible = true;
		PanelSubir->Visible = false;
		CargarAlbumesDelArtista();
	}
	private: System::Void label56_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		CargarAlbumsYSinglesEn(listBox9, true);
		panelStats->Visible = false;
		PanelConfi->Visible = false;
		panelDisco->Visible = false;
		PanelEliminar->Visible = true;
		PanelEditar->Visible = false;
		PanelSubir->Visible = false;
	}
	private: System::Void label55_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		CargarAlbumsYSinglesEn(listBox11, true);
		panelStats->Visible = false;
		PanelConfi->Visible = false;
		panelDisco->Visible = true;
		PanelEliminar->Visible = false;
		PanelEditar->Visible = false;
		PanelSubir->Visible = false;
	}
	private: System::Void label54_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		panelStats->Visible = true;
		PanelConfi->Visible = false;
		panelDisco->Visible = false;
		PanelEliminar->Visible = false;
		PanelEditar->Visible = false;
		PanelSubir->Visible = false;
	}
	private: System::Void label51_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		panelStats->Visible = false;
		PanelConfi->Visible = true;
		panelDisco->Visible = false;
		PanelEliminar->Visible = false;
		PanelEditar->Visible = false;
		PanelSubir->Visible = false;
		CargarDatosArtistaEnCampos();
	}
	private: System::Void label53_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		panelArtista->Visible = false;
		panel1->Visible = true;
	}

		   // Fin estetica y mostrar paneles superpuestos 
		   //=====================================================================================// 


			// PLAY usuario
	private: System::Void pictureBox36_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (!PlayFromList(listBox1)) {
			MessageBox::Show("Selecciona una canción del catálogo.", "Aviso");
		}
	}
		   // PAUSA usuario
	private: System::Void pictureBox35_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (axWindowsMediaPlayer2 != nullptr) axWindowsMediaPlayer2->Ctlcontrols->pause();
	}

		   // Agregar una Cancion nueva o album 
	private: System::Void label41_MouseClick(System::Object^, System::Windows::Forms::MouseEventArgs^) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Imágenes (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
		ofd->Title = "Seleccione una imagen";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				System::Drawing::Image^ img = System::Drawing::Image::FromFile(ofd->FileName);
				pictureBox58->Image = img;
				pictureBox58->SizeMode = PictureBoxSizeMode::Zoom;
			}
			catch (System::Exception^ ex) {
				MessageBox::Show("Error al cargar la imagen:\n" + ex->Message, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}
		   // Hacer que se ponga una cancion de cualquier arreglo dentro de cualquier listbox para el artista 
	private: System::Void pictureBox53_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (listBox6 && listBox6->SelectedIndex >= 0 && PlayFromLB(listBox6)) return;
		if (listBox12 && listBox12->SelectedIndex >= 0 && PlayFromLB(listBox12)) return;
		if (listBox11 && listBox11->SelectedIndex >= 0 && PlayFromLB(listBox11)) return;
		if (listBox10 && listBox10->SelectedIndex >= 0 && PlayFromLB(listBox10)) return;
		if (listBox7 && listBox7->SelectedIndex >= 0 && PlayFromLB(listBox7)) return;
		if (listBox1 && listBox1->SelectedIndex >= 0 && PlayFromLB(listBox1)) return;
		System::Windows::Forms::MessageBox::Show("Selecciona una canción en alguna lista.");
	}
		   // PAUSA del artista 
	private: System::Void pictureBox54_Click(System::Object^ sender, System::EventArgs^ e) {
		if (axWindowsMediaPlayer2 == nullptr) return;
		try { axWindowsMediaPlayer2->Ctlcontrols->pause(); }
		catch (...) {}
	}
		   // Boton para publicar un single o Album 
	private: System::Void label48_MouseClick(System::Object^, System::Windows::Forms::MouseEventArgs^) {
		if (listBox6->Items->Count == 0) { MessageBox::Show("No hay pistas agregadas."); return; }

		// Falta de metadatos por clave del listBox
		auto faltantes = gcnew System::Collections::Generic::List<System::String^>();
		for each (System::Object ^ it in listBox6->Items) {
			System::String^ key = it->ToString();
			if (!pistasListas->ContainsKey(key)) faltantes->Add(key);
		}
		if (faltantes->Count > 0) {
			MessageBox::Show("Completa los datos de todas las pistas antes de publicar.\nFaltan: "
				+ System::String::Join(", ", faltantes));
			return;
		}

		if (checkBox3->Checked) {
			if (listBox6->Items->Count != 1) {
				MessageBox::Show("Single: debe haber exactamente una pista.");
				return;
			}
			System::String^ key = listBox6->Items[0]->ToString();
			Cancion^ c = pistasListas[key];

			ListaCanciones->Agregar(c);
			if (artistaActual) artistaActual->AgregarCancion(c);

			MessageBox::Show("Single publicado.");
		}
		else {
			// Álbum: exige nombre
			if (textBox7 == nullptr || System::String::IsNullOrWhiteSpace(textBox7->Text)) {
				System::Windows::Forms::MessageBox::Show("Escribe el nombre del álbum.");
				textBox7->Focus();
				return;
			}

			// Álbum: portada seleccionada en el MISMO PictureBox (no desde la listBox)
			System::Drawing::Image^ portadaAlbum = (pictureBox58) ? pictureBox58->Image : nullptr;
			if (portadaAlbum == nullptr || portadaAlbum == x2) {
				System::Windows::Forms::MessageBox::Show(
					"Selecciona la portada del álbum con el botón 'Seleccionar Imagen' (mismo PictureBox).");
				return;
			}

			Album^ alb = EnsureAlbumEnEdicion(textBox7->Text, portadaAlbum,
				/*genero*/ nullptr, /*categoria*/ nullptr);
			if (!alb) { System::Windows::Forms::MessageBox::Show("No fue posible crear/obtener el álbum."); return; }

			// Agregar pistas al álbum y a la lista general
			for each (System::Object ^ it in listBox6->Items) {
				System::String^ key = it->ToString();
				Cancion^ c = pistasListas[key];
				alb->AgregarCancion(c);
				ListaCanciones->Agregar(c);
			}

			alb->Publicado = true;
			if (artistaActual && !artistaActual->BuscarAlbumPorId(alb->Id))
				artistaActual->AgregarAlbum(alb);

			System::Windows::Forms::MessageBox::Show("Álbum publicado.");
		}


		// Limpieza total
		NoPublicadas = gcnew ::ListaCanciones();
		pistasListas->Clear();
		songPaths->Clear(); trackCovers->Clear();
		listBox6->Items->Clear(); if (label92) label92->Text = "0 Archivos";
		if (textBox7) textBox7->Clear();
		if (textBox14) textBox14->Clear(); if (textBox15) textBox15->Clear();
		if (comboBox1) comboBox1->SelectedIndex = -1; if (comboBox3) comboBox3->SelectedIndex = -1;
		if (pictureBox58) pictureBox58->Image = x2;

		// Refresca tus paneles
		CargarAlbumsYSinglesEn(listBox11, true);
		CargarAlbumsYSinglesEn(listBox9, true);
		CargarAlbumsYSinglesEn(listBox8, true);
		CargarAlbumesDelArtista();
	}


	private: System::Void listBox8_SelectedIndexChanged(System::Object^, System::EventArgs^) {
		if (!listBox8 || listBox8->SelectedIndex < 0) return;

		if (IsSinglesBucket(listBox8->SelectedItem)) {
			albumSel = nullptr;
			MostrarAlbumEnUI(nullptr);
			CargarSinglesEnListBox7();

			if (listBox7->Items->Count > 0) {
				listBox7->SelectedIndex = 0;
				if (auto s0 = dynamic_cast<SongRef^>(listBox7->Items[0])) {
					MostrarCancionEnUI(GetCancionById(s0->id));
				}
				else {
					MostrarCancionEnUI(nullptr);
				}
			}
			else {
				MostrarCancionEnUI(nullptr);
			}
			return;
		}

		if (auto aRef = dynamic_cast<AlbumRef^>(listBox8->SelectedItem)) {
			albumSel = GetAlbum(aRef);
			MostrarAlbumEnUI(albumSel);
			CargarCancionesDeAlbumEn(albumSel, listBox7);

			if (listBox7->Items->Count > 0) {
				listBox7->SelectedIndex = 0;
				if (auto s0 = dynamic_cast<SongRef^>(listBox7->Items[0]))
					MostrarCancionEnUI(GetCancionById(s0->id));
				else
					MostrarCancionEnUI(nullptr);
			}
			else {
				MostrarCancionEnUI(nullptr);
			}
		}
	}
	private: System::String^ MakeUniqueTitle(System::String^ baseTitle) {
		if (System::String::IsNullOrWhiteSpace(baseTitle)) baseTitle = "(sin título)";
		System::String^ candidate = baseTitle;
		int i = 1;
		while (true) {
			bool exists = false;
			for each (System::Object ^ it in this->listBox6->Items) {
				if (System::String::Equals(candidate, it->ToString(), System::StringComparison::OrdinalIgnoreCase)) {
					exists = true; break;
				}
			}
			if (!exists) return candidate;
			candidate = baseTitle + " (" + i++.ToString() + ")";
		}
	}
		   // Del album seleccionado se muestran sus canciones 
	private: System::Void listBox7_SelectedIndexChanged(System::Object^, System::EventArgs^) {
		if (!listBox7 || listBox7->SelectedIndex < 0) return;
		Cancion^ c = nullptr;
		if (auto sRef = dynamic_cast<SongRef^>(listBox7->SelectedItem)) {
			c = (albumSel) ? albumSel->BuscarCancionPorId(sRef->id)
				: GetCancionById(sRef->id);
		}
		MostrarCancionEnUI(c);
	}

		   // Editar una cancion Artista 
	private: System::Void label61_MouseClick(System::Object^, System::Windows::Forms::MouseEventArgs^) {
		if (!cancionSel) { MessageBox::Show("Seleccione una canción."); return; }

		System::String^ nuevoTitulo = textBox17 ? textBox17->Text : nullptr;
		System::String^ nuevaDesc = textBox28 ? textBox28->Text : nullptr;
		System::String^ nuevoGenero = (comboBox5 && comboBox5->SelectedItem) ? comboBox5->SelectedItem->ToString() : nullptr;
		System::String^ nuevaCat = (comboBox4 && comboBox4->SelectedItem) ? comboBox4->SelectedItem->ToString() : nullptr;
		System::Drawing::Image^ nuevaImg = pictureBox60 ? pictureBox60->Image : nullptr;

		if (System::String::IsNullOrWhiteSpace(nuevoTitulo)) { MessageBox::Show("Título requerido."); return; }
		if (System::String::IsNullOrWhiteSpace(nuevoGenero)) { MessageBox::Show("Seleccione género."); return; }
		if (System::String::IsNullOrWhiteSpace(nuevaCat)) { MessageBox::Show("Seleccione categoría."); return; }

		cancionSel->setTitulo(nuevoTitulo);
		cancionSel->setDescripcion(nuevaDesc);
		cancionSel->setGenero(nuevoGenero);
		cancionSel->setCategoria(nuevaCat);
		if (nuevaImg) cancionSel->setRutaImagen(nuevaImg);

		int idx = listBox7 ? listBox7->SelectedIndex : -1;
		if (idx >= 0) {
			if (auto sref = dynamic_cast<SongRef^>(listBox7->Items[idx])) sref->text = nuevoTitulo;
			else listBox7->Items[idx] = nuevoTitulo;
		}

		MessageBox::Show("Canción actualizada.");
	}

		   // Cargar albumes para mostrar sus canciones 
	private: System::Void listBox9_SelectedIndexChanged(System::Object^, System::EventArgs^) {
		albumSel = nullptr; cancionSel = nullptr;
		if (!listBox9 || listBox9->SelectedIndex < 0) { if (listBox10) listBox10->Items->Clear(); return; }

		if (auto sRef = dynamic_cast<SingleRef^>(listBox9->SelectedItem)) {
			CargarCancionSingleEn(sRef->songId, listBox10);
			return;
		}

		if (auto aRef = dynamic_cast<AlbumRef^>(listBox9->SelectedItem)) {
			albumSel = GetAlbum(aRef);
			CargarCancionesDeAlbumEn(albumSel, listBox10);
		}
	}

		   // Cuandoo se selecciona una cancion se guarda cual es 
	private: System::Void listBox10_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		cancionSel = nullptr;
		if (!listBox10 || listBox10->SelectedIndex < 0) return;

		auto sRef = dynamic_cast<SongRef^>(listBox10->SelectedItem);
		if (sRef != nullptr) {
			cancionSel = GetCancionById(sRef->id);
		}
	}

		   // Eliminar una Cancion 
	private: System::Void label64_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (!listBox9 || listBox9->SelectedIndex < 0) {
			System::Windows::Forms::MessageBox::Show("Seleccione un álbum o single.");
			return;
		}

		if (listBox10 && listBox10->SelectedIndex >= 0) {
			auto sRef = dynamic_cast<SongRef^>(listBox10->SelectedItem);
			if (sRef == nullptr) return;

			Cancion^ c = GetCancionById(sRef->id);
			System::String^ nombreCancion = c ? c->getTitulo() : listBox10->SelectedItem->ToString();

			auto respSong = System::Windows::Forms::MessageBox::Show(
				"¿Quieres eliminar la canción \"" + nombreCancion + "\"?\nEsta acción no se puede deshacer.",
				"Confirmar eliminación",
				System::Windows::Forms::MessageBoxButtons::YesNo,
				System::Windows::Forms::MessageBoxIcon::Warning,
				System::Windows::Forms::MessageBoxDefaultButton::Button2
			);
			if (respSong != System::Windows::Forms::DialogResult::Yes) return;

			Album^ alb = nullptr;
			if (auto aRef = dynamic_cast<AlbumRef^>(listBox9->SelectedItem)) alb = GetAlbum(aRef);

			EliminarCancionEverywhere(sRef->id, alb);

			if (alb) CargarCancionesDeAlbum(alb);
			else     CargarCancionSingle(sRef->id);

			if (alb && alb->Pistas && alb->Pistas->Contar() == 0) {
				auto r = System::Windows::Forms::MessageBox::Show(
					"El álbum quedó vacío. ¿Eliminar álbum \"" + alb->Titulo + "\"?",
					"Confirmar",
					System::Windows::Forms::MessageBoxButtons::YesNo,
					System::Windows::Forms::MessageBoxIcon::Question,
					System::Windows::Forms::MessageBoxDefaultButton::Button2
				);
				if (r == System::Windows::Forms::DialogResult::Yes) {
					artistaActual->EliminarAlbumPorId(alb->Id);
					listBox10->Items->Clear();
				}
			}
		}
		else {
			if (auto aRef = dynamic_cast<AlbumRef^>(listBox9->SelectedItem)) {
				Album^ alb = GetAlbum(aRef);
				if (alb == nullptr) return;

				auto respAlb = System::Windows::Forms::MessageBox::Show(
					"¿Eliminar el álbum \"" + alb->Titulo + "\" y todas sus canciones?",
					"Confirmar eliminación",
					System::Windows::Forms::MessageBoxButtons::YesNo,
					System::Windows::Forms::MessageBoxIcon::Warning,
					System::Windows::Forms::MessageBoxDefaultButton::Button2
				);
				if (respAlb != System::Windows::Forms::DialogResult::Yes) return;

				auto pistas = alb->Pistas ? alb->Pistas->ToList()
					: gcnew System::Collections::Generic::List<Cancion^>();
				for each (Cancion ^ c in pistas) {
					if (c) EliminarCancionEverywhere(c->getIdUnico(), alb);
				}
				artistaActual->EliminarAlbumPorId(alb->Id);
				listBox10->Items->Clear();
			}

			else if (auto sRef = dynamic_cast<SingleRef^>(listBox9->SelectedItem)) {
				Cancion^ c = GetCancionById(sRef->songId);
				System::String^ nombre = c ? c->getTitulo() : listBox9->SelectedItem->ToString();

				auto respSingle = System::Windows::Forms::MessageBox::Show(
					"¿Eliminar el single \"" + nombre + "\"?",
					"Confirmar eliminación",
					System::Windows::Forms::MessageBoxButtons::YesNo,
					System::Windows::Forms::MessageBoxIcon::Warning,
					System::Windows::Forms::MessageBoxDefaultButton::Button2
				);
				if (respSingle != System::Windows::Forms::DialogResult::Yes) return;

				EliminarCancionEverywhere(sRef->songId, nullptr);
				listBox10->Items->Clear();
			}
		}

		CargarAlbumsYSinglesEn(listBox11, true);
		CargarAlbumsYSinglesEn(listBox9, true);
		CargarAlbumsYSinglesEn(listBox8, true);
		CargarAlbumesDelArtista();
		if (listBox9->Items->Count > 0) listBox9->SelectedIndex = 0;
	}

	private: System::Void listBox11_SelectedIndexChanged(System::Object^, System::EventArgs^) {
		albumSel = nullptr; cancionSel = nullptr;
		if (!listBox11 || listBox11->SelectedIndex < 0) { if (listBox12) listBox12->Items->Clear(); return; }

		if (auto aRef = dynamic_cast<AlbumRef^>(listBox11->SelectedItem)) {
			albumSel = GetAlbum(aRef);
			CargarCancionesDeAlbumEn(albumSel, listBox12);
		}
		else if (auto sRef = dynamic_cast<SingleRef^>(listBox11->SelectedItem)) {
			CargarCancionSingleEn(sRef->songId, listBox12);
		}
	}

		   // Hace que la accion del click funcione 
	private: System::Void label61_MouseClick_1(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		label61_MouseClick(sender, e);
	}

		   // Editar datos de usuario
	private: System::Void button11_Click(System::Object^ sender, System::EventArgs^ e) {
		if (artistaActual == nullptr) {
			MessageBox::Show("No hay una sesión de artista activa.", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		String^ usuario = textBox26 ? textBox26->Text->Trim() : "";
		String^ correo = textBox25 ? textBox25->Text->Trim() : "";
		String^ nomArt = textBox24 ? textBox24->Text->Trim() : "";
		String^ nombre = textBox23 ? textBox23->Text->Trim() : "";
		String^ pass = textBox22 ? textBox22->Text : "";
		String^ confirmar = textBox21 ? textBox21->Text : "";
		String^ generoSel = (comboBox2 && comboBox2->SelectedItem) ? comboBox2->SelectedItem->ToString() : nullptr;
		System::Drawing::Image^ nuevaImg = pictureBox63 ? pictureBox63->Image : nullptr;

		bool ok = true;
		bool changed = false;
		if (!String::IsNullOrWhiteSpace(usuario) &&
			!usuario->Equals(artistaActual->Usuario, StringComparison::Ordinal)) {
			if (ListaArtistas) {
				auto n = ListaArtistas->Buscar(nullptr, usuario);
				if (n != nullptr && n->data != artistaActual) {
					show("Ese nombre de usuario ya está en uso.", ok);
				}
			}
			if (ok) { artistaActual->Usuario = usuario; changed = true; }
		}
		if (!String::IsNullOrWhiteSpace(correo) &&
			!correo->Equals(artistaActual->CorreoElectronico, StringComparison::OrdinalIgnoreCase)) {

			String^ patronEmail = "^([\\w\\.\\-]+)@([\\w\\-]+\\.)+([A-Za-z]{2,})$";
			if (!Regex::IsMatch(correo, patronEmail)) {
				show("Correo electrónico inválido.", ok);
			}
			else {
				artistaActual->CorreoElectronico = correo; changed = true;
			}
		}
		if (!String::IsNullOrWhiteSpace(nomArt) &&
			!nomArt->Equals(artistaActual->NombreArtistico, StringComparison::Ordinal)) {
			artistaActual->NombreArtistico = nomArt; changed = true;
		}
		if (!String::IsNullOrWhiteSpace(nombre) &&
			!nombre->Equals(artistaActual->NombreReal, StringComparison::Ordinal)) {
			artistaActual->NombreReal = nombre; changed = true;
		}
		if (!String::IsNullOrWhiteSpace(generoSel) &&
			!generoSel->Equals(artistaActual->Genero, StringComparison::Ordinal)) {
			artistaActual->Genero = generoSel; changed = true;
		}
		if (!String::IsNullOrEmpty(pass) || !String::IsNullOrEmpty(confirmar)) {
			if (pass->Length < 6) {
				show("La contraseña debe tener al menos 6 caracteres.", ok);
			}
			else if (pass != confirmar) {
				show("La confirmación no coincide.", ok);
			}
			else {
				artistaActual->Contrasena = pass; changed = true;
			}
		}
		if (nuevaImg != nullptr) {
			artistaActual->ImagenPerfil = nuevaImg; changed = true;
		}
		if (!ok) return;
		if (label16)  label16->Text = "Bienvenido de nuevo, " + artistaActual->NombreArtistico;
		if (pictureBox5 && artistaActual->ImagenPerfil) pictureBox5->Image = artistaActual->ImagenPerfil;
		if (textBox22) textBox22->Clear();
		if (textBox21) textBox21->Clear();
		MessageBox::Show(changed ? "Datos actualizados correctamente."
			: "No se realizaron cambios.",
			"Éxito", MessageBoxButtons::OK, MessageBoxIcon::Information);
	}
		   // Eliminar cuenta de Artista Boton 
	private: System::Void button10_Click(System::Object^ sender, System::EventArgs^ e) {
		if (artistaActual == nullptr) {
			MessageBox::Show("No hay una sesión de artista activa.", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		auto resp = MessageBox::Show("Se eliminarán tu cuenta, todos tus álbumes y todas tus canciones. Esta acción no se puede deshacer.\n\n¿Deseas continuar?", "Confirmar eliminación", MessageBoxButtons::YesNo, MessageBoxIcon::Warning, MessageBoxDefaultButton::Button2);
		if (resp != System::Windows::Forms::DialogResult::Yes) return;
		auto singles = (artistaActual->Canciones) ? artistaActual->Canciones->ToList()
			: gcnew System::Collections::Generic::List<Cancion^>();
		for each (Cancion ^ c in singles) {
			if (c) EliminarCancionEverywhere(c->getIdUnico(), nullptr);
		}
		auto albums = AlbList();
		for each (Album ^ a in albums) {
			if (!a) continue;
			auto pistas = (a->Pistas) ? a->Pistas->ToList()
				: gcnew System::Collections::Generic::List<Cancion^>();
			for each (Cancion ^ c in pistas) {
				if (c) EliminarCancionEverywhere(c->getIdUnico(), a);
			}
			artistaActual->EliminarAlbumPorId(a->Id);
		}
		if (ListaArtistas) {
			ListaArtistas->EliminarPorId(artistaActual->Id);
		}
		artistaActual = nullptr;
		if (panelArtista) panelArtista->Visible = false;
		if (panel1)       panel1->Visible = true;
		if (textBox26) textBox26->Clear();
		if (textBox25) textBox25->Clear();
		if (textBox24) textBox24->Clear();
		if (textBox23) textBox23->Clear();
		if (textBox22) textBox22->Clear();
		if (textBox21) textBox21->Clear();
		if (comboBox2) {
			comboBox2->SelectedIndex = -1;
			comboBox2->Text = "";
		}
		if (pictureBox63)
			pictureBox63->Image = nullptr;
		MessageBox::Show("Tu cuenta y todo tu contenido fueron eliminados.", "Hecho",
			MessageBoxButtons::OK, MessageBoxIcon::Information);
	}

		   // Selecciona Imagenes en la parte de editar Artista 
	private: System::Void button9_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Imágenes (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
		ofd->Title = "Seleccione una imagen";
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			try {
				System::Drawing::Image^ img = System::Drawing::Image::FromFile(ofd->FileName);
				pictureBox63->Image = img;
				pictureBox63->SizeMode = PictureBoxSizeMode::Zoom;
			}
			catch (System::Exception^ ex) {
				MessageBox::Show("Error al cargar la imagen:\n" + ex->Message, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}
	}

	private: DraftSong^ GetSelectedDraft() {
		if (!listBox6 || listBox6->SelectedIndex < 0) return nullptr;
		return dynamic_cast<DraftSong^>(listBox6->SelectedItem);
	}

	private: System::Void btnAgregarAudios_Click(System::Object^, System::EventArgs^) {
		if (songPaths == nullptr)    songPaths = gcnew System::Collections::Generic::Dictionary<System::String^, System::String^>();
		if (trackCovers == nullptr)  trackCovers = gcnew System::Collections::Generic::Dictionary<System::String^, System::Drawing::Image^>();
		if (pistasListas == nullptr) pistasListas = gcnew System::Collections::Generic::Dictionary<System::String^, Cancion^>();
		if (checkBox3 && checkBox3->Checked && listBox6 && listBox6->Items->Count >= 1) {
			MessageBox::Show("Single: ya hay una pista cargada. Quita la pista o desmarca 'Single' para agregar más.");
			return;
		}
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->Filter = "Audio|*.mp3;*.wav;*.wma;*.aac;*.m4a;*.flac";
		ofd->Title = "Selecciona audio(s)";
		ofd->Multiselect = !checkBox3->Checked;

		if (ofd->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

		cli::array<System::String^>^ files = ofd->FileNames;
		if (checkBox3->Checked && files->Length > 1) {
			MessageBox::Show("Single: solo una pista. Se tomará el primer archivo.");
			files = gcnew cli::array<System::String^>(1) { ofd->FileName };
		}

		System::String^ lastTitle = nullptr;
		for each (System::String ^ path in files) {
			System::String^ baseName = System::IO::Path::GetFileNameWithoutExtension(path);
			System::String^ title = MakeUniqueTitle(baseName);

			songPaths[title] = CleanPath(path);
			listBox6->Items->Add(title);
			lastTitle = title;

			// Nueva pista = aún no “completa”
			if (pistasListas->ContainsKey(title)) pistasListas->Remove(title);
		}

		if (label92) label92->Text = listBox6->Items->Count.ToString() + " Archivos";

		if (!System::String::IsNullOrEmpty(lastTitle)) {
			int idx = listBox6->Items->IndexOf(lastTitle);
			if (idx >= 0) listBox6->SelectedIndex = idx;
			if (textBox14) textBox14->Text = lastTitle;
		}
		UpdateAddButtonState();
		UpdateAlbumNameState();
	}

	private: System::Void draftCategoria_Changed(System::Object^, System::EventArgs^) {
		auto d = GetSelectedDraft(); if (!d) return;
		d->Categoria = comboBox3 && comboBox3->SelectedItem ? comboBox3->SelectedItem->ToString() : nullptr;
		listBox6->Invalidate();
	}

	private: System::Void draftDescripcion_Changed(System::Object^, System::EventArgs^) {
		auto d = GetSelectedDraft(); if (!d) return;
		d->Descripcion = textBox15 ? textBox15->Text : nullptr;
	}

	private: System::Void checkBox3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		single = checkBox3->Checked;
		textBox7->Enabled = !single;
		if (single) textBox7->Text = "";
	}
	private: System::Void listBox6_SelectedIndexChanged_1(System::Object^ sender, System::EventArgs^ e) {
		bool hasSel = (listBox6->SelectedIndex >= 0);

		btnAgregarAudios->Enabled = !hasSel;
		textBox7->Enabled = !checkBox3->Checked && !hasSel;

		if (!hasSel) {
			textBox14->Text = "";
			textBox15->Text = "";
			comboBox1->SelectedIndex = -1;
			comboBox3->SelectedIndex = -1;
			pictureBox58->Image = x2;
			return;
		}

		System::String^ key = listBox6->SelectedItem->ToString();
		if (textBox14) textBox14->Text = key;

		if (pistasListas->ContainsKey(key)) {
			Cancion^ c = pistasListas[key];
			textBox14->Text = c->getTitulo();
			textBox15->Text = c->getDescripcion();
			SelectComboByText(comboBox1, c->getGenero());
			SelectComboByText(comboBox3, c->getCategoria());
			if (c->getRutaImagen()) pictureBox58->Image = c->getRutaImagen();
			else pictureBox58->Image = x2;
		}
		else if (trackCovers->ContainsKey(key)) {
			pictureBox58->Image = trackCovers[key];
		}
		else {
			pictureBox58->Image = x2;
		}
	}

	private: System::Void listBox6_MeasureItem_1(System::Object^ sender, System::Windows::Forms::MeasureItemEventArgs^ e) {
		e->ItemHeight = 68;
	}
	private: System::Void listBox6_DrawItem_1(System::Object^ sender, System::Windows::Forms::DrawItemEventArgs^ e) {
		if (e->Index < 0) return;
		e->DrawBackground();

		String^ title = listBox6->Items[e->Index]->ToString();
		Image^ img = trackCovers->ContainsKey(title) ? trackCovers[title] : nullptr;

		bool isSel = ((e->State & DrawItemState::Selected) == DrawItemState::Selected);
		System::Drawing::Color bg = isSel
			? System::Drawing::Color::FromArgb(0x44, 0x79, 0x67)
			: System::Drawing::Color::FromArgb(0x30, 0x33, 0x3B);

		using namespace System::Drawing;
		e->Graphics->FillRectangle(gcnew SolidBrush(bg), e->Bounds);

		// miniatura
		System::Drawing::Rectangle rcImg(e->Bounds.Left + 6, e->Bounds.Top + 6, 56, 56);
		if (img) e->Graphics->DrawImage(img, rcImg);
		else     e->Graphics->DrawRectangle(Pens::DimGray, rcImg);

		// título
		System::Drawing::Rectangle rcText(e->Bounds.Left + 72, e->Bounds.Top + 6, e->Bounds.Width - 78, 56);
		auto font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
		auto br = gcnew SolidBrush(System::Drawing::Color::White);
		e->Graphics->DrawString(title, font, br, rcText);

		e->DrawFocusRectangle();
	}
	private: System::Void label58_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (listBox6->SelectedIndex < 0) { MessageBox::Show("Selecciona una pista de la lista."); return; }

		String^ itemKey = listBox6->SelectedItem->ToString();
		if (!songPaths->ContainsKey(itemKey)) { MessageBox::Show("Ruta de audio no encontrada."); return; }

		// Validar metadatos
		String^ titulo = textBox14->Text->Trim();
		String^ genero = (comboBox1->SelectedItem) ? comboBox1->SelectedItem->ToString() : nullptr;
		String^ cat = (comboBox3->SelectedItem) ? comboBox3->SelectedItem->ToString() : nullptr;
		String^ desc = textBox15->Text;
		Image^ cover = pictureBox58->Image;

		bool ok = true;
		if (String::IsNullOrWhiteSpace(titulo)) { MessageBox::Show("Título requerido."); ok = false; }
		if (String::IsNullOrWhiteSpace(genero)) { MessageBox::Show("Selecciona un género."); ok = false; }
		if (String::IsNullOrWhiteSpace(cat)) { MessageBox::Show("Selecciona una categoría."); ok = false; }
		if (cover == nullptr || cover == x2) { MessageBox::Show("Selecciona una portada."); ok = false; }
		if (!ok) return;

		String^ ruta = CleanPath(songPaths[itemKey]);

		double dur = 0.0;
		try {
			auto wmp = gcnew WMPLib::WindowsMediaPlayerClass();
			auto media = wmp->newMedia(ruta);
			if (media) dur = media->duration;
			wmp->close();
		}
		catch (...) {}

		String^ fechaStr = System::DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss");
		Cancion^ c = gcnew Cancion(
			generarMusica(),
			titulo,
			this->artistaActual ? this->artistaActual->NombreArtistico : "",
			genero,
			cat,
			dur,
			desc,
			ruta,
			cover,
			true,
			fechaStr,
			checkBox3->Checked
		);
		if (Cancion^ old = FindByTitle(NoPublicadas, itemKey)) {
			NoPublicadas->EliminarPorId(old->getIdUnico());
		}
		NoPublicadas->Agregar(c);
		pistasListas[itemKey] = c;
		trackCovers[itemKey] = cover;
		listBox6->Invalidate();
		listBox6->ClearSelected();
		btnAgregarAudios->Enabled = true;
		textBox7->Enabled = !checkBox3->Checked;
		textBox14->Clear(); textBox15->Clear();
		comboBox1->SelectedIndex = -1; comboBox3->SelectedIndex = -1;
		pictureBox58->Image = x2;
	}

	private: System::Void label58_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		this->label58->BackColor = System::Drawing::Color::PaleVioletRed;
	}
	private: System::Void label58_MouseLeave(System::Object^ sender, System::EventArgs^ e) {

		this->label58->BackColor = System::Drawing::Color::FromArgb(64, 64, 64);
	}
	private: System::Void label95_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		auto r = System::Windows::Forms::MessageBox::Show(
			"¿Cancelar y borrar todas las pistas cargadas?",
			"Cancelar", MessageBoxButtons::YesNo, MessageBoxIcon::Question);
		if (r != System::Windows::Forms::DialogResult::Yes) return;

		LimpiarCargaActual();
	}
	private: System::Void listBox12_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		cancionSel = nullptr;
		if (!listBox12 || listBox12->SelectedIndex < 0) {
			MostrarCancionEnUI(nullptr);
			return;
		}
		if (auto sRef = dynamic_cast<SongRef^>(listBox12->SelectedItem)) {
			cancionSel = GetCancionById(sRef->id);
		}
		MostrarCancionEnUI(cancionSel);
	}
	private: System::Void AxPlayer_PlayStateChange(System::Object^ sender,
		AxWMPLib::_WMPOCXEvents_PlayStateChangeEvent^ e)
	{
		if (e->newState == 3) {
			mediaDur = (this->axWindowsMediaPlayer2->currentMedia)
				? this->axWindowsMediaPlayer2->currentMedia->duration : 0.0;
			if (mediaDur < 0) mediaDur = 0;
			if (this->label75) this->label75->Text = FmtTime(mediaDur);
			playTimer->Start();
		}
		else if (e->newState == 2) {
			playTimer->Stop();
		}
		else if (e->newState == 1) {
			playTimer->Stop();
			if (this->label75) this->label75->Text = "0:00";
			if (this->progressBar2) this->progressBar2->Value = 0;
			if (this->prFill) this->prFill->Width = 0;
		}
		else if (e->newState == 8) {
			playTimer->Stop();
			NextTrack();
		}
	}

	private: System::Void PlayTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
		double pos = 0.0;
		try { pos = this->axWindowsMediaPlayer2->Ctlcontrols->currentPosition; }
		catch (...) {}

		double remain = (mediaDur > 0 ? (mediaDur - pos) : 0.0);
		if (remain < 0) remain = 0;

		if (this->label75) this->label75->Text = FmtTime(remain);

		int percent = (mediaDur > 0) ? (int)System::Math::Round((pos / mediaDur) * 100.0) : 0;
		if (percent < 0) percent = 0; if (percent > 100) percent = 100;

		if (this->progressBar2) {
			this->progressBar2->Value = percent;
			if (this->prFill) {
				double frac = (mediaDur > 0 ? pos / mediaDur : 0.0);
				int fullW = this->progressBar2->Width;
				this->prFill->Width = (int)System::Math::Round(fullW * frac);
			}
		}
	}

	private: System::Void pictureBox52_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		PrevTrack();
	}
	private: System::Void pictureBox51_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		NextTrack();
	}
	private: System::Void pictureBox56_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (axWindowsMediaPlayer2 == nullptr) return;
		try { axWindowsMediaPlayer2->Ctlcontrols->pause(); }
		catch (...) {}
	}
};
}