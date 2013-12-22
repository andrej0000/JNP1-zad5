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

/*
	TODO:
		-wypisywanie wyjatkow poprawic
		-throw-guranatee
*/

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>

#ifdef DEBUG
	const static bool debug = true;
#else
	const static bool debug = true;
#endif


using std::vector;
using std::map;
using std::set;
using std::pair;
using std::shared_ptr;
using std::weak_ptr;

class VirusNotFound : public std::exception{
public:
	virtual const char* what() const noexcept {
		return "VirusNotFound";
	}
};

class VirusAlreadyCreated : public std::exception{
public:
	virtual const char* what() const noexcept {
		return "VirusAlreadyCreated";
	}
};

class TriedToRemoveStemVirus : public std::exception{
public:
	virtual const char* what() const noexcept {
		return "TriedToRemoveStemVirus";
	}
};


template <class Virus>
class VirusGenealogy{
private:
	class Node;

	typedef shared_ptr<Node> s_ptr;
	typedef weak_ptr<Node> w_ptr;
	typedef map <typename Virus::id_type, w_ptr> map_type;
	typedef pair <typename Virus::id_type, w_ptr> map_pair;

	class Node {
	public:
		Virus vir;
		typename Virus::id_type id;

		set<w_ptr, std::owner_less<w_ptr>> parents;
		set<s_ptr> children;
		Node(Virus v) : vir(v), id(v.get_id())
		{
		}
		~Node(){
			if (debug)
				std::cout << "Destruktor ~Node of id: " << id << std::endl;
		}
		Virus& get_virus(){
			return vir;
		}
		void add_parent(s_ptr p){
			//-T-O-D-O-: co jesli sie nie uda stworzyc w_ptr
			// uda, mamy gwarantowane noexept. insert moze nie pojsc
			w_ptr w(p);
			parents.insert(w);
		}

		void add_child(s_ptr c){
			if (debug)
				std::cout << "Dodano dziecko " << c->id << " do " << this->id<< std::endl;
			children.insert(c);
		}

		void remove_child(s_ptr c){
			if(debug)
				std::cout << "usunieto dziecko " << c->id << " z " << id << std::endl;
			children.erase(c);
		}

		vector <typename Virus::id_type> get_parents(){
			vector <typename Virus::id_type> vid;
			for ( auto it = parents.begin(); it != parents.end(); it++){
				if (s_ptr sp = it->lock()){
					vid.push_back(sp->id);
				}
				else {
					parents.erase(it);
				}
			}
			return vid;
		}

		vector <typename Virus::id_type> get_children(){
			vector <typename Virus::id_type> vid;
			for (typename set<s_ptr>::iterator it = children.begin(); it != children.end(); it++){
				if ((*it)){
					vid.push_back((*it)->id);
				}
				else {
					children.erase(it);
				}
			}
			return vid;
		}
	};
	const typename Virus::id_type stem_id;

	s_ptr stem;
	map_type nodes;

	s_ptr get_virus_by_id(typename Virus::id_type const &id) const {
		auto it = this->nodes.find(id);
		if (it == this->nodes.end()){
			throw VirusNotFound();
		}
		if (s_ptr sp = it->second.lock()){
			return sp;
		}
		else {
			throw VirusNotFound();
		}
	}

public:
	VirusGenealogy( const VirusGenealogy& ) = delete;
	VirusGenealogy& operator=( const VirusGenealogy& ) = delete;

	// Tworzy nową genealogię.
	// Tworzy także węzęł wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(typename Virus::id_type const &stem_id) : stem(new Node(Virus(stem_id))){
		nodes.insert(map_pair(stem_id, w_ptr(stem)));
	}

	// Zwraca identyfikator wirusa macierzystego.
	typename Virus::id_type get_stem_id() const {
		return stem->id;
	}

	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<typename Virus::id_type> get_children(typename Virus::id_type const &id) const {
		s_ptr s(get_virus_by_id(id));
		return s->get_children();
	};

	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const {
		s_ptr s = get_virus_by_id(id);
		return s->get_parents();
	}

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(typename Virus::id_type const &id) const {
		auto it = this->nodes.find(id);
		return (it != this->nodes.end() && it->second.lock());
	/*
		try {
			this->get_virus_by_id(id);
			return true;
		}
		catch (VirusNotFound vnf){
			return false;
		}
	*/
	};

	// Zwraca referencję do obiektu reprezentującego wirus o podanym
	// identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	Virus& operator[](typename Virus::id_type const &id) const {
		s_ptr s(get_virus_by_id(id));
		return s->get_virus();
	}

	// Tworzy węzęł reprezentujący nowy wirus o identyfikatorze id
	// powstały z wirusów o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
	// id już istnieje.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	// poprzedników nie istnieje.
	void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id){
		if (exists(id)){
			throw VirusAlreadyCreated();
		}
		s_ptr s(new Node(Virus(id)));
		s_ptr parent_p = get_virus_by_id(parent_id);
		s->add_parent(parent_p);
		parent_p->add_child(s);
		nodes.insert(map_pair(id, w_ptr(s)));
	}
	void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids){
		if (exists(id)){
			throw VirusAlreadyCreated();
		}

		vector<s_ptr> parents;
		for( auto parent_id : parent_ids )
			parents.push_back( get_virus_by_id(parent_id) );

		s_ptr new_node( new Node(Virus(id)) );

		try{
			for( auto parent : parents ){
				parent->add_child(new_node);
				new_node->add_parent(parent);
			}
			nodes.insert( map_pair(id, w_ptr(new_node) ) );
		} catch( std::exception e ){
			for( auto parent : parents )
				parent->remove_child(new_node); // nie rzuca
		}
		/*
		s_ptr s(new Node(Virus(id)));
		for (auto parent_id : parent_ids){
			s_ptr parent_p = get_virus_by_id(parent_id);
			s->add_parent(parent_p);
			parent_p->add_child(s);
		}
		nodes.insert(map_pair(id, w_ptr(s)));
		*/
	}

	// Dodaje nową krawędź w grafie genealogii.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
	void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id){
		s_ptr child = get_virus_by_id(child_id);
		s_ptr parent = get_virus_by_id(parent_id);
		parent->add_child(child);
		try {
			child->add_parent(parent);
		} catch( std::exception e ){
			parent->remove_child(child);
		}
	}

	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(typename Virus::id_type const &id){
		if (id == get_stem_id()){
			throw TriedToRemoveStemVirus();
		}
		s_ptr source = get_virus_by_id(id);

		try{
			for( auto p : source->parents )
				if( s_ptr parent = p.lock() )
					parent->remove_child(source);

			nodes.erase(id);
		}catch( std::exception e ){ // rollback
			for( auto p : source->parents )
				if( s_ptr parent = p.lock() )
					parent->add_child(source);
		}
		/*
		auto it = nodes.find(id);
		if (nodes.end() == it){
			throw VirusNotFound();
		}
		else {
			if (s_ptr source = it->second.lock()){
				for (w_ptr p : source->parents){
					if (s_ptr parent = p.lock()){
						parent->children.erase(source);
					}
				}
			}
			nodes.erase(it);
		}
		*/
	}
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
