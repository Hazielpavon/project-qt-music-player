#pragma once
using namespace System;
using namespace System::Drawing;
public ref class Usuarios
{
private:
    int       id_;
    String^ username_;
    String^ encryptedPassword_;
    String^ realName_;
    String^ email_;
    DateTime registrationDate_;
    bool      isActive_;
    Image^ avatarPath_;

public:
    Usuarios(int        id,
        String^ username,
        String^ encryptedPassword,
        String^ realName,
        String^ email,
        DateTime registrationDate,
        bool       isActive,
        Image^ avatarPath);

    // Getters
    int      getId();
    String^ getUsername();
    String^ getEncryptedPassword();
    String^ getRealName();
    String^ getEmail();
    DateTime getRegistrationDate();
    bool     getIsActive();
    Image^ getAvatarPath();

    // Setters
    void setId(int id);
    void setUsername(String^ username);
    void setEncryptedPassword(String^ encryptedPassword);
    void setRealName(String^ realName);
    void setEmail(String^ email);
    void setRegistrationDate(DateTime registrationDate);
    void setIsActive(bool isActive);
    void setAvatarPath(Image^ avatarPath);
};
