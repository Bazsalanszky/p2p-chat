//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14.
//

#pragma  once
#define  _CRT_SECURE_NO_WARNINGS
#include "../utility.h"
#include "../lib/tcp-listener.h"
#include "peer.h"
#include "config.h"
#include "../lib/debugmalloc/debugmalloc.h"


/*!
 * @brief A felhasználói felület számára szükséges adatokat tárolja
 */
typedef struct WebIO{    //!A felhasználói felület socket-e
    SOCKET socket;       //! A mappa amiben a felhasználói felület fájljai vannak
    char folder[65];    //! A peerek listájának mutatója
    struct PeerList * list;
} WebIO;
/*!
 * @brief A kapott adatok alapján létrehozza a felhasználói felületet
 * @param[in] config A program konfigurációja lásd config.h
 * @param[in] list A peerek listájának mutatója
 * @param[out] webIo Mutató arra struktúrára ahová az adatokat írni szeretnénk
 * @return A művelet sikerességét jelző szám
 * <table>
 * <tr><th>Kód</th><th>Jelentése</th></tr>
 * <tr><td>0</td><td>A művelet sikeres volt</td></tr>
 * <tr><td>1</td><td>A bind() függvény futása közben hiba lépett fel</td></tr>
 * <tr><td>2</td><td>A listen() függvény futása közben hiba lépett fel</td></tr>
 * <tr><td>3</td><td>A tcp_createIPv4Socket() függvény futása közben hiba lépett fel</td></tr>
 * </table>
 */
int webio_create(Config config, struct PeerList *list, WebIO *webIo);
/*!
 * @brief Fogadja a socket-re érkező csatlakozásokat és a HTTP request-ek alapján elküldi a HTTP response-okat
 * @param[in] wio A felhasználói felület struktúrája
 * @return A művelet sikerességét jelző szám.(Ha 0, akkor a sikeres volt,egyébként sikertelen)
 */
int webio_handleRequest(WebIO wio);
/*!
 * @brief Ez a függvény kezeli a HTTP GET request-eket. Elküldi a kért filokat.
 * @param[in] client A socket amire az adatokat kell küldeni
 * @param[in] wio A felhasználói felület struktúrája
 * @param[in] file A kért file neve
 * @return A művelet sikerességét jelző szám.(Ha 0, akkor a sikeres volt,egyébként sikertelen)
 */
static int webio_handleGETrequest(SOCKET client, WebIO wio, char *file);
/*!
 * @brief Ez a függvény kezeli a HTTP POST request-eket.
 * @param[in] client A socket amire az adatokat kell küldeni
 * @param[in] wio A felhasználói felület struktúrája
 * @param[in] post A kapott adatokat tároló Map
 * @return A művelet sikerességét jelző szám.(Ha 0, akkor a sikeres volt,egyébként sikertelen)
 */
static int webio_handlePOSTrequest(SOCKET client, WebIO wio, Map post);
/*!
 * @brief a file neve alapján visszaadja a típusához tartozó <a href="https://en.wikipedia.org/wiki/MIME">MIME típust</a>
 * @param[in] filename A file neve
 * @return A file típusához tartozó <a href="https://en.wikipedia.org/wiki/MIME">MIME típus</a>
 */
static char* webio_getMIMEtype(char* filename);
/*!
 * Visszaadja a paraméterként kapott fájl kiterjesztését
 * @param[in] filename A fájl neve
 * @return A fájl kiterjesztése
 * @note A visszatérési érték címét a malloc() függvénnyel foglalja le,így később fel kell azt szabadítani
 */
static char* webio_getFiletype(char* filename);
/*!
 * @brief Betölti a fejlécet a főbb oldalakhoz
 * @param[in] folder A mappa amiben a fejléc fájl(header.html) található
 * @param[out] result A fejléc tartalma
 */
static void webio_getHeader(char* folder, char result[]);
/*!
 * @brief Betölti a főoldal tartalmát
 * @param[in] wio A felhasználói felület struktúrája
 * @param outputBuffer Ebbe a buffer-be fogja a főoldalt betölteni
 */
static void webio_getIndex(WebIO wio, char *outputBuffer);
/*!
 * @brief Betölti egy megadott peerhez tartozó oldal tartalmát
 * @param[in] wio A felhasználói felület struktúrája
 * @param[in] id A peer,aminek az oldalát szeretnénk betölteni
 * @param[out] outputBuffer Ebbe a buffer-be fogja az oldalt betölteni
 */
static void webio_getPeerPage(WebIO wio, char *id, char *outputBuffer);
/*!
 * @brief Megnézi hogy egy adott azonosítójú peer létezik.e
 * @param[in] folder A felhasználói felület mappája
 * @param[in] id A peer azonosítója amit keresünk
 * @return Igaz vagy hamis érték az alapján hogy a peer létezik-e( vagy létezett)
 */
static bool webio_isPeerFound(char* folder,char* id);
/*!
 * @brief Elküldi a HTTP header-t,ami azt jelzi fogy a kért file létezik
 * @param[in] socket A socket amire a header-t szeretnénk küldeni
 * @param[in] file A kért file neve
 * @note Ha ismerjük a file(vagyis az oldal) méretét, érdemesebb a webio_sendOKHeader_wSize() függvényt használni
 */
static void webio_sendOKHeader(SOCKET socket,char* file);
/*!
 * @copybrief webio_sendOKHeader
 * @param[in] socket A socket amire a header-t szeretnénk küldeni
 * @param[in] file A kért file neve
 * @param[in] size A kért file mérete
 */
static void webio_sendOKHeader_wSize(SOCKET socket,char* file,int size);
/*!
 * @brief Elküld a egy oldalt
 * @param[in] socket A socket amire az oldalt küldeni szeretnénk
 * @param[in] content Az oldal tartalma
 */
static void webio_sendPage(SOCKET socket, char* content);
/*!
 * @brief Elküldi a HTTP header-t,ami azt jelzi fogy a kért file nem létezik
 * @param[in] socket A socket amire a header-t szeretnénk küldeni
 */
static void webio_send404Page(SOCKET socket);

static void getOfflineMessages(WebIO wio, char *content);

static void sendFile(char* path, SOCKET client);