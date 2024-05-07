package nsu.urdin.chatprotocol.entity;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.Serializable;

@Getter
@AllArgsConstructor
public class User implements Serializable {
    private final String name;
}
