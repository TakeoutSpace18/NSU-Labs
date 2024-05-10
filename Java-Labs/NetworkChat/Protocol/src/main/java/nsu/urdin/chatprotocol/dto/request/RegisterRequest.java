package nsu.urdin.chatprotocol.dto.request;

import lombok.Getter;

@Getter
public class RegisterRequest extends RequestBase {
    private final String name;
    private final String password;

    public RegisterRequest(String name, String password) {
        super("register");
        this.name = name;
        this.password = password;
    }
}
