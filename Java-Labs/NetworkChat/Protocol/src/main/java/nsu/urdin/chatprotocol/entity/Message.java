package nsu.urdin.chatprotocol.entity;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;

import java.io.Serializable;

@Builder
@Getter
@AllArgsConstructor
public class Message implements Serializable {
    private String userName;
    private String text;
}
