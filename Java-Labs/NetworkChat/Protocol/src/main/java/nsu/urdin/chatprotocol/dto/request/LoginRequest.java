package nsu.urdin.chatprotocol.dto.request;

import lombok.Getter;

@Getter
public class LoginRequest extends RequestBase {
    private final String name;
    private final String password;

    public LoginRequest(String name, String password) {
        super("login");
        this.name = name;
        this.password = password;
    }
}
