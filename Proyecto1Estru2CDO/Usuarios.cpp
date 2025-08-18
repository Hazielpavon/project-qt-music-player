#include "Usuarios.h"

Usuarios::Usuarios(int       id,
    String^ username,
    String^ encryptedPassword,
    String^ realName,
    String^ email,
    DateTime registrationDate,
    bool      isActive,
    Image^ avatarPath)
    : id_(id),
    username_(username),
    encryptedPassword_(encryptedPassword),
    realName_(realName),
    email_(email),
    registrationDate_(registrationDate),
    isActive_(isActive),
    avatarPath_(avatarPath)
{ }

// Getters

int Usuarios::getId() {
    return id_;
}

String^ Usuarios::getUsername() {
    return username_;
}

String^ Usuarios::getEncryptedPassword() {
    return encryptedPassword_;
}

String^ Usuarios::getRealName() {
    return realName_;
}

String^ Usuarios::getEmail() {
    return email_;
}

DateTime Usuarios::getRegistrationDate() {
    return registrationDate_;
}

bool Usuarios::getIsActive() {
    return isActive_;
}

Image^ Usuarios::getAvatarPath() {
    return avatarPath_;
}

// Setters

void Usuarios::setId(int id) {
    id_ = id;
}

void Usuarios::setUsername(String^ username) {
    username_ = username;
}

void Usuarios::setEncryptedPassword(String^ encryptedPassword) {
    encryptedPassword_ = encryptedPassword;
}

void Usuarios::setRealName(String^ realName) {
    realName_ = realName;
}

void Usuarios::setEmail(String^ email) {
    email_ = email;
}

void Usuarios::setRegistrationDate(DateTime registrationDate) {
    registrationDate_ = registrationDate;
}

void Usuarios::setIsActive(bool isActive) {
    isActive_ = isActive;
}

void Usuarios::setAvatarPath(Image^ avatarPath) {
    avatarPath_ = avatarPath;
}
