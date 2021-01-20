# CLON

## Crazy Lisp Object Notation

***Crazy Lisp Object Notation*** (**CLON**) est un format de données textuelles dérivé de la syntaxe des langages de programmation Lisp. Il permet de représenter l'information structurée comme le permet le XML ou le JSON. L'idée du **CLON** est venue de plusieurs constats :
- Le XML est beaucoup trop verbeux et donc peu économe en ressources lors de la transmission de données par les infrastructures réseaux ou encore les stockages sur disque dur. 
- Le JSON est plus succint que le XML à écrire, mais aussi plus complexe à parser. Il demande donc davantage de ressources CPU pour être lu. 

Et ces deux formats de données ne sont que deux exemples parmi d'autres. CLON vient tenter d'apporter une amélioration à ses deux constats. CLON offre une syntaxe extrêmement simple à lire par un humain et par un ordinateur ainsi qu'une verbosité réduite pour minimiser l'impact sur la mémoire. 

### Syntaxe

Un document CLON est une structuration récursive de plusieurs types de valeurs : 
- **string** : représente une chaine de caractère de taille quelconque entourée de guillemets. 
- **number** : représente un nombre qu'il soit entier ou flottant, positif ou négatif. 
- **boolean** : représente deux valeurs *vrai* et *faux* (*true* et *false*) pour la logique booléenne.
- **object** : représente une liste de documents CLON qu'ils soient de type *string*, *number*, *boolean* ou même *object* (permet la récursivité structurelle).

Chaque document CLON se décompose en trois parties, un nom, un index et une valeur. Le nom et l'index permettent de localiser une valeur au sein de la liste entière de document CLON. Quant à la valeur, elle contient la donnée que l'on veut représenter. 

Rien n'empèche d'avoir plusieurs valeurs d'avoir le même nom. Ce qui les distinguera alors sera leur index propre. Il est à noter que l'index est implicite. Il n'apparait dans la liste qu'au travers de l'emplacement physique du document au sein de la liste d'appartenance. 

### Exemple

Prenons un exemple pour illustrer tout cela :

```lisp
(person
  (name "Doe")
  (firstname "John")
  (age 30)
  (adress 
    (street "4 John Smith Avenue")
    (city "New Pariss")
    (code "35435"))
  (adress 
    (street "5 John Smith Avenue")
    (city "New Pariss")
    (code "35435")))
```

Nous avons représenté une personne avec deux adresses, l'une au numéro 4 et l'autre au numéro 5 de la John Smith Avenue. Ces deux adresses ont pour localisation `adress:4` et `adress:5`, les index 4 et 5 étant l'ordre dans lequel ces deux adresses apparaissent dans la liste des documents dans le document `person`. 

### Utilisation

Le CLON peut être utilisé pour : 
- la sérialisation et la dé-sérialisation d'objets. 
- l'encodage de données structurées. 
- les fichiers de configurations.
- le stockage d'information en base de données. 
- l'échange de données entre systèmes. 
- la préparation à la restitution de données à un humain. 
- la communication interprocess. 
- la génération de logs applicatifs structurés.
- et bien d'autres choses encore. 

# Quick start



# Makefile lifecycle

Le cycle de vie de ce makefile reste classique pour simplifier la prise en main de ce projet. 

The makefile lifecycle of this project is classical to simplify *la prise en main* of this project. 

- **version** : searches the `VERSION` in the *clon.hpp* file to set the `${VARIABLE}`.
- **clean** : removes all generated files in the project directory.
- **build** : builds every *.o* files used to build the main *libclon.o* file.
- **test** : launches all the unit tests of the library.
- **dist** : creates an zip archive with the results of build and if the tests are all OK. 
- **bench** : runs the benchmarks tests and outputs the statistics.
- **install** : 
- **clean-temporaries** : just clean temporary generated files. 

# API reference

## Model

```c++
enum struct clon_type
{
  none,
  boolean,
  number,
  string,
  object
};

using object = std::vector<node>;

using clon_value = std::variant<
    boolean, number, string, object>;

struct node
{
  std::string_view name;
  clon_value val;
};

struct root
{
  std::vector<char> buff;
  node root;
};
```

Ces définitions représentent le modèle fondamental de la librairie. Le `clon_type` est l'ensemble des types que les valeurs clon peuvent prendre. 
`node` représente un noeud au sein de la structure récursive d'une donnée clon. `root` 



## Parsing



## Path

## Transformation

# Contributing
 
