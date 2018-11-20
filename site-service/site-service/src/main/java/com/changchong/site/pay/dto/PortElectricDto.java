package com.changchong.site.pay.dto;

import lombok.Data;

import java.io.Serializable;
import java.math.BigDecimal;

@Data
public class PortElectricDto implements Serializable{
    private Integer portCode;
    private BigDecimal currentElectric;
}
