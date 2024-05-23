package nsu.urdin.chatprotocol.dto;

import lombok.Getter;

import java.io.Serializable;

@Getter
public class DtoBase implements Serializable {
    public String className;

    public DtoBase() {
        className = this.getClass().getName();
    }
}
