#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_REGISTROS 1000
// Deslocamento para criptografia simples usando Cifra de César
#define SHIFT 3  


// Struct para armazenar os dados dos users
typedef struct {
    int id;
    char nome[50];
    int idade;
    char senha[50];
    int isAdmin;  // 1 para admin, 0 para comum
} Registro;


// Variável para controle de login
int logado = 0;  

//Variável para armazenar as infos do usuário logado
Registro usuarioLogado; 


//Procedimento para criptografar as senhas
void criptografar_senha(char* senha) {
    for (int i = 0; senha[i] != '\0'; i++) {
        senha[i] += SHIFT;  
    }
}

//Procedimento para descriptografar a senha 
void descriptografar_senha(char* senha) {
    for (int i = 0; senha[i] != '\0'; i++) {
        senha[i] -= SHIFT; 
    }
}

// Função para validar a senha (deve conter número e caractere especial)
int validar_senha(char* senha) {
    int temNumero = 0, temEspecial = 0;

    for (int i = 0; senha[i] != '\0'; i++) {
        if (isdigit(senha[i])) {
            temNumero = 1;
        } else if (!isalnum(senha[i])) { // Verifica se é um caractere especial
            temEspecial = 1;
        }
    }

    return temNumero && temEspecial;
}


// Procedimento para carregar registros no txt
void load_from_file(Registro registros[]) {
    FILE *file = fopen("registros.txt", "r");
    if (file != NULL) {
        int i = 0;
        while (fscanf(file, "%d %s %d %s %d", &registros[i].id, registros[i].nome, &registros[i].idade, registros[i].senha, &registros[i].isAdmin) != EOF) {
            // Descriptografa a senha ao carregar
            descriptografar_senha(registros[i].senha);
            i++;
        }
        fclose(file);
    }
}

//Procedimento para salvar registros no txt
void save_to_file(Registro registros[]) {
    FILE *file = fopen("registros.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < MAX_REGISTROS; i++) {
            if (registros[i].id != 0) { // Se tiver vazio, ele não grava.
                criptografar_senha(registros[i].senha); 
                fprintf(file, "%d %s %d %s %d\n", registros[i].id, registros[i].nome, registros[i].idade, registros[i].senha, registros[i].isAdmin);
                // Descriptografa a senha depois de salvar
                descriptografar_senha(registros[i].senha);
            }
        }
        fclose(file);
    }
}

//Procedimento que cria um novo registro de usuario
void create(Registro registros[], char nome[], int idade, char senha[], int isAdmin) {
    if (!validar_senha(senha)) {
        printf("Senha inválida. A senha deve conter ao menos um número e um caractere especial.\n");
        return;
    }

    Registro novoRegistro;
    int id = 0;

    //o FOR garante que um novo usuário pegue um id disponível a partir de 1
    for (int i = 0; i < MAX_REGISTROS; i++) {
        if (registros[i].id == 0) {  
            id = i + 1;
            break;
        }
    }

    novoRegistro.id = id;
    strcpy(novoRegistro.nome, nome);
    novoRegistro.idade = idade;
    strcpy(novoRegistro.senha, senha);
    novoRegistro.isAdmin = isAdmin;

    // Criptografa a senha antes de salvar
    criptografar_senha(novoRegistro.senha);

    // Salvar o novo usuario
    registros[id - 1] = novoRegistro;
    save_to_file(registros); 
    printf("Registro criado com sucesso!\n");
}

// procedimento pra ler os registros
void read(Registro registros[]) {
    for (int i = 0; i < MAX_REGISTROS; i++) {
        if (registros[i].id != 0) {
            printf("ID: %d, Nome: %s, Idade: %d, Senha (criptografada): %s, Admin: %d\n", 
                   registros[i].id, registros[i].nome, registros[i].idade, registros[i].senha, registros[i].isAdmin);
        }
    }
}


// Esse procedimento garante o update, que é atualizar registro
void update(Registro registros[], int id, char nome[], int idade, char senha[], int isAdmin) {
    if (strlen(senha) > 0 && !validar_senha(senha)) {
        printf("Senha inválida. A senha deve conter ao menos um número e um caractere especial.\n");
        return;
    }

    for (int i = 0; i < MAX_REGISTROS; i++) {
        if (registros[i].id == id) {
            strcpy(registros[i].nome, nome);
            registros[i].idade = idade;

            // Verificando se trocou de senha. Se sim, criptografa ela
            if (strlen(senha) > 0) {
                strcpy(registros[i].senha, senha);
                criptografar_senha(registros[i].senha); 
            }

            registros[i].isAdmin = isAdmin;
            save_to_file(registros);
            printf("Registro atualizado com sucesso!\n");
            return;
        }
    }
    printf("Registro não encontrado.\n");
}


//Deletar um registro
void delete(Registro registros[], int id) {
    for (int i = 0; i < MAX_REGISTROS; i++) {
        if (registros[i].id == id) {
            registros[i].id = 0; 
            save_to_file(registros);
            printf("Registro deletado com sucesso!\n");
            return;
        }
    }
    printf("Registro não encontrado.\n");
}

//Login
void login(Registro registros[]) {
    char nome[50], senha[50];
    int found = 0;

    while (!found) {
        printf("Nome: ");
        scanf("%s", nome);
        printf("Senha: ");
        scanf("%s", senha);

        for (int i = 0; i < MAX_REGISTROS; i++) {
            // Descriptografa a senha do usuário e compara com a digitada
            char senhaDescriptografada[50];
            strcpy(senhaDescriptografada, registros[i].senha);
            descriptografar_senha(senhaDescriptografada);

            if (strcmp(registros[i].nome, nome) == 0 && strcmp(senhaDescriptografada, senha) == 0) {
                usuarioLogado = registros[i];
                logado = 1;
                found = 1;
                printf("Login bem-sucedido!\n");
                break;
            }
        }

        //se der errado o login, o usuário volta pra tela inicial
        if (!found) {
            printf("Login falhou. Tente novamente.\n");
            return;  
        }
    }
}

// Menu inicial de cadastro/login
void menu_cadastro(Registro registros[]) {
    int escolha;
    while (!logado) {
        printf("\nMenu de Login/Cadastro:\n");
        printf("1. Login\n2. Cadastrar novo usuário\n3. Sair\nEscolha uma opção: ");
        scanf("%d", &escolha);

        if (escolha == 1) {
            login(registros);
        } else if (escolha == 2) {
            char nome[50], senha[50];
            int idade, isAdmin;
            printf("Nome: ");
            scanf("%s", nome);
            printf("Idade: ");
            scanf("%d", &idade);
            printf("Senha: ");
            scanf("%s", senha);
            printf("Você é um administrador? (1 para sim, 0 para não): ");
            scanf("%d", &isAdmin);

            create(registros, nome, idade, senha, isAdmin); 
        } else if (escolha == 3) {
            printf("Saindo...\n");
            exit(0);
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

//Menu principal para users logados
void menu_principal(Registro registros[]) {
    int escolha;
    while (logado) {
        printf("\nMenu Principal:\n");

        if (usuarioLogado.isAdmin == 0) {
            printf("1. Visualizar Registros\n2. Sair\nEscolha uma opção: ");
            scanf("%d", &escolha);

            if (escolha == 1) {
                read(registros);
            } else if (escolha == 2) {
                logado = 0;
                printf("Saindo...\n");
            } else {
                printf("Opção inválida. Tente novamente.\n");
            }
        } else {
            printf("1. Visualizar Registros\n2. Criar Novo Registro\n3. Atualizar Registro\n4. Deletar Registro\n5. Sair\nEscolha uma opção: ");
            scanf("%d", &escolha);

            if (escolha == 1) {
                read(registros);
            } else if (escolha == 2) {
                // Exemplo de criar um novo usuário
                char nome[50], senha[50];
                int idade, isAdmin;
                printf("Nome: ");
                scanf("%s", nome);
                printf("Idade: ");
                scanf("%d", &idade);
                printf("Senha: ");
                scanf("%s", senha);
                printf("Você é um administrador? (1 para sim, 0 para não): ");
                scanf("%d", &isAdmin);

                create(registros, nome, idade, senha, isAdmin);
            } else if (escolha == 3) {
                int id;
                char nome[50], senha[50];
                int idade, isAdmin;

                printf("ID do usuário para atualizar: ");
                scanf("%d", &id);
                printf("Novo Nome: ");
                scanf("%s", nome);
                printf("Nova Idade: ");
                scanf("%d", &idade);
                printf("Nova Senha: ");
                scanf("%s", senha);
                printf("Novo status de Admin (1 para sim, 0 para não): ");
                scanf("%d", &isAdmin);

                update(registros, id, nome, idade, senha, isAdmin);
            } else if (escolha == 4) {
                int id;
                printf("ID do usuário para excluir: ");
                scanf("%d", &id);
                delete(registros, id);
            } else if (escolha == 5) {
                logado = 0;
                printf("Saindo...\n");
            } else {
                printf("Opção inválida. Tente novamente.\n");
            }
        }
    }
}

// Função principal
int main() {
    Registro registros[MAX_REGISTROS] = {0};  
    load_from_file(registros);  

    menu_cadastro(registros);  
    menu_principal(registros);

    return 0;
}
