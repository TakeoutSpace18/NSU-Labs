package nsu.urdin.chatprotocol.dto;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.Serializable;

@AllArgsConstructor
@Getter
public class ReplyBase implements Serializable {
    private final String status;
}
