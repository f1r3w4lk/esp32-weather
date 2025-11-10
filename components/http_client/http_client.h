#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file http_client.h
 * @brief Módulo genérico para requisições HTTP (GET e POST) com ESP-IDF.
 *
 * Este módulo fornece funções simplificadas para enviar requisições HTTP
 * e armazenar as respostas em um buffer fornecido pelo usuário.
 *
 * Ele suporta HTTPS com o bundle interno de certificados da ESP-IDF.
 */

/**
 * @brief Executa uma requisição HTTP GET.
 *
 * @param url              URL completa do recurso (ex: "https://api.open-meteo.com/v1/...").
 * @param response_buffer  Buffer de destino para armazenar o corpo da resposta.
 * @param max_len          Tamanho máximo do buffer de resposta.
 *
 * @return
 *  - ESP_OK: sucesso.
 *  - ESP_ERR_INVALID_ARG: argumentos inválidos.
 *  - ESP_FAIL: falha geral.
 *  - Outros códigos de erro do cliente HTTP.
 */
esp_err_t http_get(const char *url, char *response_buffer, size_t max_len);

/**
 * @brief Executa uma requisição HTTP POST.
 *
 * @param url              URL completa do destino (ex: "https://api.callmebot.com/...").
 * @param post_data        Corpo da requisição (geralmente JSON).
 * @param response_buffer  Buffer de destino para armazenar a resposta do servidor.
 * @param max_len          Tamanho máximo do buffer de resposta.
 *
 * @return
 *  - ESP_OK: sucesso.
 *  - ESP_ERR_INVALID_ARG: argumentos inválidos.
 *  - ESP_FAIL: falha geral.
 *  - Outros códigos de erro do cliente HTTP.
 */
esp_err_t http_post(const char *url, const char *post_data, char *response_buffer, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif  // HTTP_CLIENT_H
