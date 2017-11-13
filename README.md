# CryptEight  

Projet réalisé dans le cadre du cours de cryptographie, promotion 5A MOC à l'ESGI (Paris).  
Le but du projet est de créer un dispositif d'échange de messages sécurisés entre un client et un serveur.  
Le langage imposé étant le C, nous avons utilisé l'interface réseau proposée par les sockets.  
L'échange se déroule ainsi :  
+ Le client crée une clé composée de 5 lettres aléatoires et l'envoi au serveur.  
+ Le serveur crée également sa clé aléatoire, la concatène avec celle du client, et renvoi le tout.  
+ Le client et le serveur partagent ainsi une clé mutuelle. Les échanges peuvent commencer.
    - Le message est chiffré avec la clé mutuelle grâce à un simple XOR.  
    - À partir du message non chiffré et de la clé mutuelle, une nouvelle clé est générée :  
        * La nouvelle clé est créée à partir de l'entrelacement (1 caractère sur 2) du message et de la clé actuelle.  
        * Même si le message fait plus de 10 caractères, la clé a une taille fixe de 10 caractères.  
    - Une fois la nouvelle clé générée côté client, le message chiffré est envoyé au serveur.  
    - Le serveur déchiffre le message (avec l'ancienne clé), et met à jour sa clé en procédant à la même opération que le client.  
+ Les échanges sont ainsi sécurisés avec un premier échange de clés mutuelles et un renouvellement de clé à chaque envoi de message.  
  
Le programme a été fait en 3 jours dans le cadre d'une semaine thématique.  
