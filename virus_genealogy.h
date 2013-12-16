//=== Zadanie Genealogia Wirusów ===

/* Biolog, zajmujący się konstruowaniem wirusów, potrzebuje klasy do
manipulowania informacjami o ich genealogii. Nowy wirus powstaje przez
mutację jakiegoś znanego wirusa. Może też powstać przez zmutowanie
połączenia więcej niż jednego wirusa. Genealogia wirusa jest grafem
skierowanym acyklicznym o jednym źródle, które reprezentuje wirusa
macierzystego. Wierzchołek grafu odpowiada konkretnej mutacji wirusa.
Krawędź łączy wirusa z bezpośrednio otrzymaną z niego mutacją. */

// Należy zaimplementować szablon klasy, która reprezentuje taki graf.
/* Klasa Virus reprezentuje informacje o wirusie. Jej implementacja
zostanie dostarczona w stosownym czasie.
Klasa VirusGenealogy powinna udostępniać następujący interfejs. */

/*
Zakładamy, że:
* klasa Virus ma konstruktor przyjmujący argument typu Virus::id_type;
* klasa Virus ma metodę Virus::id_type get_id() const;
* typ Virus::id_type ma konstruktor bezargumentowy, konstruktor
  kopiujący, operator przypisania;
* wartości typu Virus::id_type tworzą porządek liniowy i można je
  porównywać za pomocą operatorów ==, !=, <=, >=, <, >.
*/

#include <iostream>
#include <vector>
#include <map>
#include <memory>

using std::vector;
using std::map;
using std::shared_ptr;
using std::weak_ptr;

class VirusNotFound : public std::exception{
public:
	virtual char const * what() noexcept {
		return "VirusNotFound";
	}
};

class VirusAlreadyCreated : public std::exception{
public:
	virtual char const * what() noexcept {
		return "VirusAlreadyCreated";
	}
};

class TriedToRemoveStemVirus : public std::exception{
public:
	virtual char const * what() noexcept {
		return "TriedToRemoveStemVirus";
	}
};


template <class Virus>
class VirusGenealogy{
private:

	class Node {
	public:
		typedef shared_ptr<Node> s_ptr;
		typedef weak_ptr<Node> w_ptr;

		Virus vir;
		typename Virus::id_type id;

		vector<w_ptr> parents;
		vector<s_ptr> children;
		Node(Virus v) : vir(v), id(v.get_id())
		{
		}
		//TODO: konstruktory
		//TODO: dodawanie parentow/childrenow
		//TODO: getter do id, parents children
	};
	typename Virus::id_type const stem_id;
	//TODO: trzymanie w jakiejs mapie wszystkich node'ow z kluczem type_id
	//TODO: getter do stem_id pewnie
public:
	// Tworzy nową genealogię.
	// Tworzy także węzęł wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(typename Virus::id_type const &stem_id);

	// Zwraca identyfikator wirusa macierzystego.
	typename Virus::id_type get_stem_id() const;

	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<typename Virus::id_type> get_children(typename Virus::id_type const &id) const;

	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const;

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(typename Virus::id_type const &id) const;

	// Zwraca referencję do obiektu reprezentującego wirus o podanym
	// identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	Virus& operator[](typename Virus::id_type const &id) const;

	// Tworzy węzęł reprezentujący nowy wirus o identyfikatorze id
	// powstały z wirusów o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
	// id już istnieje.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	// poprzedników nie istnieje.
	void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id);
	void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids);

	// Dodaje nową krawędź w grafie genealogii.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
	void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id);

	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(typename Virus::id_type const &id);
};

/*
Ponadto:
* wszystkie metody klasy VirusGenealogy powinny gwarantować silną odporność
  na wyjątki, a tam, gdzie to jest możliwe i pożądane, powinny być no-throw;
* próba użycia konstruktora kopiującego lub operatora przypisania dla
  obiektów klasy VirusGenealogy powinna zakończyć się błędem kompilacji;
* zachowanie obiektu typu VirusGenealogy po utworzeniu cyklu pozostaje
  niezdefiniowane -- nie trzeba wykrywać takiej sytuacji;
* wyjątki VirusAlreadyCreated, VirusNotFound oraz TriedToRemoveStemVirus
  powinny być zdefiniowane poza klasą VirusGenealogy i powinny dziedziczyć
  z std::exception;
* wyszukiwanie wirusów powinno być szybsze niż liniowe.

Zarządzanie pamięcią powinno być zrealizowane za pomocą inteligentnych
wskaźników tak, żeby wierzchołek był dealokowany w momencie usunięcia
ostatniego jego poprzednika. Można to zrealizować, używając odpowiednich
szablonów z biblioteki, bądź przez zaimplementowanie własnego takiego
szablonu.
*/

/*
template <class T> class SharedPtr {
  public:
    // Tworzy pusty wskaźnik, na nic niewskazujący.
    // Metoda get() powinna zwrócić wskaźnik 0 (NULL, nullptr).
    SharedPtr();

    // Przestaje być właścicielem wskazywanego obiektu.
    ~SharedPtr();

    // Przejmuje na własność obiekt wskazywany przez pointer.
    SharedPtr(T* pointer);

    // Staje się współwłaścicielem obiektu wskazywanego przez other.
    SharedPtr(SharedPtr<T> const &other);

    // Zwraca przechowywany wskaźnik, ale pozostaje właścicielem
    // obiektu.
    T* get();

    // Staje się (współ)właścicielem obiektu wskazywanego przez other.
    // Przestaje być właścicielem obiektu dotychczas wskazywanego.
    SharedPtr<T>& operator=(SharedPtr<T> const &other);
    SharedPtr<T>& operator=(T* other);

    // Zwraca true wtw, gdy porównywane wskaźniki wskazują na ten sam
    // obiekt.
    bool operator==(const SharedPtr<T>& other) const;

  private:
    // Tutaj już możesz umieścić, co Ci pasuje.
};
*/
