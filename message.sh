#!/bin/bash

# =========================
# Configuration
# =========================
HOST="127.0.0.1"
PORT=6667
PASS="pass"
TIMEOUT=0.5

# =========================
# Couleurs
# =========================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# =========================
# Compteurs de tests
# =========================
TESTS_TOTAL=0
TESTS_PASSED=0
TESTS_FAILED=0

# =========================
# Fonctions utilitaires
# =========================
print_section() {
    echo -e "\n${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}  $1${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

test_result() {
    local test_name=$1
    local expected=$2
    local client_log=$3
    
    ((TESTS_TOTAL++))
    
    if [ ! -f "$client_log" ]; then
        echo -e "${RED}✗${NC} $test_name ${YELLOW}(log inexistant)${NC}"
        ((TESTS_FAILED++))
        return 1
    fi
    
    if grep -q "$expected" "$client_log" 2>/dev/null; then
        echo -e "${GREEN}✓${NC} $test_name"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $test_name"
        echo -e "  ${YELLOW}Attendu: $expected${NC}"
        ((TESTS_FAILED++))
        return 1
    fi
}

# =========================
# Nettoyage automatique
# =========================
cleanup() {
    echo -e "\n${YELLOW}Nettoyage...${NC}"
    
    # Fermeture des processus nc
    for pid in $PID_ALICE $PID_BOB $PID_CHARLIE $PID_DAVE; do
        if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
            kill "$pid" 2>/dev/null
        fi
    done
    
    # Suppression des fichiers temporaires
    rm -f /tmp/irc_alice_in /tmp/irc_bob_in /tmp/irc_charlie_in /tmp/irc_dave_in
    rm -f /tmp/irc_alice.log /tmp/irc_bob.log /tmp/irc_charlie.log /tmp/irc_dave.log
    
    wait 2>/dev/null
}
trap cleanup EXIT INT TERM

# =========================
# Fonction d'envoi
# =========================
send_cmd() {
    local client=$1
    local cmd=$2
    local name=$3
    echo -e "${BLUE}→${NC} ${MAGENTA}$name${NC}: $cmd"
    echo "$cmd" >> "/tmp/irc_${client}_in"
    sleep $TIMEOUT
}

# =========================
# Initialisation
# =========================
print_section "Initialisation"
echo -e "${YELLOW}Vérification du serveur IRC...${NC}"

# Test de connexion au serveur
if ! timeout 2 bash -c "echo > /dev/tcp/$HOST/$PORT" 2>/dev/null; then
    echo -e "${RED}✗ Serveur IRC non accessible sur $HOST:$PORT${NC}"
    echo -e "${YELLOW}Assurez-vous que votre serveur IRC est démarré${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Serveur accessible${NC}"

# =========================
# Création des fichiers
# =========================
echo -e "${YELLOW}Création des fichiers de communication...${NC}"
rm -f /tmp/irc_alice_in /tmp/irc_bob_in /tmp/irc_charlie_in /tmp/irc_dave_in
rm -f /tmp/irc_alice.log /tmp/irc_bob.log /tmp/irc_charlie.log /tmp/irc_dave.log

touch /tmp/irc_alice_in /tmp/irc_bob_in /tmp/irc_charlie_in /tmp/irc_dave_in

# =========================
# Connexion des clients
# =========================
echo -e "${YELLOW}Connexion des clients au serveur...${NC}"

tail -f /tmp/irc_alice_in | nc -C "$HOST" "$PORT" > /tmp/irc_alice.log 2>&1 &
PID_ALICE=$!
sleep 0.2

tail -f /tmp/irc_bob_in | nc -C "$HOST" "$PORT" > /tmp/irc_bob.log 2>&1 &
PID_BOB=$!
sleep 0.2

tail -f /tmp/irc_charlie_in | nc -C "$HOST" "$PORT" > /tmp/irc_charlie.log 2>&1 &
PID_CHARLIE=$!
sleep 0.2

tail -f /tmp/irc_dave_in | nc -C "$HOST" "$PORT" > /tmp/irc_dave.log 2>&1 &
PID_DAVE=$!
sleep 0.5

echo -e "${GREEN}✓ Clients connectés${NC}"

# =========================
# 1. Authentification
# =========================
print_section "1. Tests d'authentification"

send_cmd alice "PASS $PASS" "Alice"
send_cmd alice "NICK Alice" "Alice"
send_cmd alice "USER alice_usr 0 * :Alice Liddell" "Alice"
sleep 1
test_result "Connexion Alice (001)" "001.*Alice" "/tmp/irc_alice.log"
test_result "Connexion Alice (bienvenue)" "Welcome" "/tmp/irc_alice.log"

send_cmd bob "PASS $PASS" "Bob"
send_cmd bob "NICK Bob" "Bob"
send_cmd bob "USER bob_usr 0 * :Bob Ross" "Bob"
sleep 1
test_result "Connexion Bob" "001.*Bob" "/tmp/irc_bob.log"

send_cmd charlie "PASS $PASS" "Charlie"
send_cmd charlie "NICK Charlie" "Charlie"
send_cmd charlie "USER charlie_usr 0 * :Charlie Brown" "Charlie"
sleep 1
test_result "Connexion Charlie" "001.*Charlie" "/tmp/irc_charlie.log"

# Test mauvais mot de passe
send_cmd dave "PASS wrongpass" "Dave"
send_cmd dave "NICK Dave" "Dave"
send_cmd dave "USER dave_usr 0 * :Dave" "Dave"
sleep 1
test_result "Refus mauvais mot de passe" "464" "/tmp/irc_dave.log"

# =========================
# 2. Tests de NICK
# =========================
print_section "2. Tests NICK"

> /tmp/irc_alice.log  # Vider le log pour test propre
send_cmd alice "NICK Alice2" "Alice"
sleep 0.5
test_result "Changement de pseudo" "NICK.*Alice2" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "NICK Alice2" "Bob"
sleep 0.5
test_result "Pseudo déjà utilisé (433)" "433" "/tmp/irc_bob.log"

send_cmd alice "NICK Alice" "Alice2"
sleep 0.5

# =========================
# 3. Tests JOIN / TOPIC
# =========================
print_section "3. Tests JOIN et TOPIC"

> /tmp/irc_alice.log
send_cmd alice "JOIN #ecole" "Alice"
sleep 0.5
test_result "Alice rejoint #ecole" "JOIN.*#ecole" "/tmp/irc_alice.log"

send_cmd alice "TOPIC #ecole :Apprendre le C++" "Alice"
sleep 0.5
test_result "Définition du topic" "TOPIC.*#ecole" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "JOIN #ecole" "Bob"
sleep 0.5
test_result "Bob rejoint #ecole" "JOIN.*#ecole" "/tmp/irc_bob.log"
test_result "Bob voit le topic" "332.*#ecole" "/tmp/irc_bob.log"

> /tmp/irc_alice.log
send_cmd alice "TOPIC #ecole" "Alice"
sleep 0.5
test_result "Récupération du topic" "332.*#ecole" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "TOPIC #nonexistant" "Bob"
sleep 0.5
test_result "Topic canal inexistant" "403" "/tmp/irc_bob.log"

# =========================
# 4. Tests PRIVMSG et NOTICE
# =========================
print_section "4. Tests PRIVMSG et NOTICE"

> /tmp/irc_bob.log
send_cmd alice "PRIVMSG Bob :Salut Bob !" "Alice"
sleep 0.5
test_result "Message privé Alice→Bob" "PRIVMSG Bob :Salut Bob" "/tmp/irc_bob.log"

> /tmp/irc_alice.log
> /tmp/irc_bob.log
send_cmd bob "PRIVMSG #ecole :Hello tout le monde" "Bob"
sleep 0.5
test_result "Message sur #ecole" "PRIVMSG #ecole :Hello" "/tmp/irc_alice.log"

> /tmp/irc_alice.log
send_cmd charlie "NOTICE Alice :Notification" "Charlie"
sleep 0.5
test_result "Notice Charlie→Alice" "NOTICE Alice :Notification" "/tmp/irc_alice.log"

> /tmp/irc_alice.log
send_cmd alice "PRIVMSG NonExistant :Test" "Alice"
sleep 0.5
test_result "Message user inexistant (401)" "401" "/tmp/irc_alice.log"

# =========================
# 5. Tests de requêtes
# =========================
print_section "5. Tests de requêtes d'informations"

> /tmp/irc_alice.log
send_cmd alice "LIST" "Alice"
sleep 0.5
test_result "Commande LIST" "322.*#ecole" "/tmp/irc_alice.log"

> /tmp/irc_alice.log
send_cmd alice "NAMES #ecole" "Alice"
sleep 0.5
test_result "Commande NAMES" "353.*#ecole" "/tmp/irc_alice.log"

> /tmp/irc_alice.log
send_cmd alice "WHOIS Bob" "Alice"
sleep 0.5
test_result "Commande WHOIS" "311.*Bob" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "WHO #ecole" "Bob"
sleep 0.5
test_result "Commande WHO" "352.*#ecole" "/tmp/irc_bob.log"

> /tmp/irc_alice.log
send_cmd alice "WHOIS FakeUser" "Alice"
sleep 0.5
test_result "WHOIS user inexistant (401)" "401" "/tmp/irc_alice.log"

# =========================
# 6. Tests de modes (invite-only)
# =========================
print_section "6. Tests de modes (invite-only)"

> /tmp/irc_alice.log
send_cmd alice "MODE #ecole +i" "Alice"
sleep 0.5
test_result "Mode +i activé" "MODE #ecole.*\\+i" "/tmp/irc_alice.log"

> /tmp/irc_charlie.log
send_cmd charlie "JOIN #ecole" "Charlie"
sleep 0.5
test_result "JOIN refusé (invite-only - 473)" "473" "/tmp/irc_charlie.log"

> /tmp/irc_alice.log
send_cmd alice "INVITE Charlie #ecole" "Alice"
sleep 0.5
test_result "INVITE Charlie" "341.*Charlie.*#ecole" "/tmp/irc_alice.log"

> /tmp/irc_charlie.log
send_cmd charlie "JOIN #ecole" "Charlie"
sleep 0.5
test_result "JOIN après INVITE" "JOIN.*#ecole" "/tmp/irc_charlie.log"

# =========================
# 7. Tests de modes (key)
# =========================
print_section "7. Tests de modes (clé)"

> /tmp/irc_alice.log
send_cmd alice "JOIN #prive" "Alice"
sleep 0.5

send_cmd alice "MODE #prive +k secret123" "Alice"
sleep 0.5
test_result "Mode +k activé" "MODE #prive.*\\+k" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "JOIN #prive" "Bob"
sleep 0.5
test_result "JOIN sans clé refusé (475)" "475" "/tmp/irc_bob.log"

> /tmp/irc_bob.log
send_cmd bob "JOIN #prive wrongkey" "Bob"
sleep 0.5
test_result "JOIN avec mauvaise clé (475)" "475" "/tmp/irc_bob.log"

> /tmp/irc_bob.log
send_cmd bob "JOIN #prive secret123" "Bob"
sleep 0.5
test_result "JOIN avec bonne clé" "JOIN.*#prive" "/tmp/irc_bob.log"

# =========================
# 8. Tests de modes (limit)
# =========================
print_section "8. Tests de modes (limite d'utilisateurs)"

> /tmp/irc_alice.log
send_cmd alice "JOIN #limite" "Alice"
sleep 0.5

send_cmd alice "MODE #limite +l 2" "Alice"
sleep 0.5
test_result "Mode +l 2 activé" "MODE #limite.*\\+l" "/tmp/irc_alice.log"

> /tmp/irc_bob.log
send_cmd bob "JOIN #limite" "Bob"
sleep 0.5
test_result "Bob rejoint (limite: 2/2)" "JOIN.*#limite" "/tmp/irc_bob.log"

> /tmp/irc_charlie.log
send_cmd charlie "JOIN #limite" "Charlie"
sleep 0.5
test_result "JOIN refusé (canal plein - 471)" "471" "/tmp/irc_charlie.log"

# =========================
# 9. Tests de modes (opérateur)
# =========================
print_section "9. Tests de modes opérateur"

> /tmp/irc_bob.log
send_cmd alice "MODE #ecole +o Bob" "Alice"
sleep 0.5
test_result "Bob devient opérateur" "MODE #ecole.*\\+o Bob" "/tmp/irc_bob.log"

> /tmp/irc_charlie.log
send_cmd charlie "MODE #ecole +o Alice" "Charlie"
sleep 0.5
test_result "Non-op ne peut pas donner +o (482)" "482" "/tmp/irc_charlie.log"

> /tmp/irc_bob.log
send_cmd bob "MODE #ecole -i" "Bob"
sleep 0.5
test_result "Bob (op) retire mode +i" "MODE #ecole.*-i" "/tmp/irc_bob.log"

# =========================
# 10. Tests KICK
# =========================
print_section "10. Tests KICK"

> /tmp/irc_charlie.log
send_cmd charlie "KICK #ecole Bob :Test" "Charlie"
sleep 0.5
test_result "Non-op ne peut pas KICK (482)" "482" "/tmp/irc_charlie.log"

> /tmp/irc_charlie.log
send_cmd alice "KICK #ecole Charlie :Au revoir" "Alice"
sleep 0.5
test_result "Alice KICK Charlie" "KICK #ecole Charlie" "/tmp/irc_charlie.log"

> /tmp/irc_alice.log
send_cmd alice "KICK #nonexistant Bob :Test" "Alice"
sleep 0.5
test_result "KICK canal inexistant (403)" "403" "/tmp/irc_alice.log"

# =========================
# 11. Tests PART
# =========================
print_section "11. Tests PART"

> /tmp/irc_bob.log
send_cmd bob "PART #prive :Je pars" "Bob"
sleep 0.5
test_result "Bob quitte #prive" "PART #prive" "/tmp/irc_bob.log"

> /tmp/irc_bob.log
send_cmd bob "PART #nonexistant" "Bob"
sleep 0.5
test_result "PART canal inexistant (403)" "403" "/tmp/irc_bob.log"

# =========================
# 12. Tests PING/PONG
# =========================
print_section "12. Tests PING"

> /tmp/irc_alice.log
send_cmd alice "PING :test_token_123" "Alice"
sleep 0.5
test_result "Réponse PONG" "PONG.*test_token_123" "/tmp/irc_alice.log"

# =========================
# 13. Tests multiples canaux
# =========================
print_section "13. Tests multi-canaux"

> /tmp/irc_alice.log
send_cmd alice "JOIN #cpp,#python,#rust" "Alice"
sleep 1
test_result "JOIN multiple (#cpp)" "JOIN.*#cpp" "/tmp/irc_alice.log"
test_result "JOIN multiple (#python)" "JOIN.*#python" "/tmp/irc_alice.log"
test_result "JOIN multiple (#rust)" "JOIN.*#rust" "/tmp/irc_alice.log"

# =========================
# 14. Tests de caractères spéciaux
# =========================
print_section "14. Tests messages spéciaux"

> /tmp/irc_bob.log
send_cmd alice "PRIVMSG Bob :Test émojis 🚀 ⚡ 💻" "Alice"
sleep 0.5
test_result "Message avec émojis" "PRIVMSG Bob :.*Test.*moji" "/tmp/irc_bob.log"

> /tmp/irc_alice.log
send_cmd bob "PRIVMSG Alice :Test français: éàç" "Bob"
sleep 0.5
test_result "Caractères accentués" "PRIVMSG Alice :.*fran" "/tmp/irc_alice.log"

# =========================
# 15. Tests QUIT
# =========================
print_section "15. Tests QUIT"

send_cmd alice "QUIT :Au revoir !" "Alice"
send_cmd bob "QUIT" "Bob"
send_cmd charlie "QUIT" "Charlie"
sleep 1

# =========================
# Résumé des tests
# =========================
print_section "Résumé des tests"

echo -e "${CYAN}Tests totaux:${NC}   $TESTS_TOTAL"
echo -e "${GREEN}Tests réussis:${NC} $TESTS_PASSED"
echo -e "${RED}Tests échoués:${NC}  $TESTS_FAILED"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}★★★ Tous les tests sont passés ! ★★★${NC}\n"
    exit 0
else
    PERCENTAGE=$((TESTS_PASSED * 100 / TESTS_TOTAL))
    echo -e "\n${YELLOW}Taux de réussite: ${PERCENTAGE}%${NC}"
    echo -e "${RED}✗ Certains tests ont échoué${NC}\n"
    exit 1
fi