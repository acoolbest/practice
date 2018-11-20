package com.changchong.site.serviceImpl;

import com.changchong.site.mapper.MicroImageMapper;
import com.changchong.site.service.MicroImageService;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

/**
 * @Description:
 * @Author: zhoumin
 * @Date: created in 2018-03-19 13:52
 **/
@Service("microImageService")
public class MicroImageServiceImpl implements MicroImageService {
    @Resource
    private MicroImageMapper microImageMapper;

    @Override
    public List<String> queryMicroImages() {
        List<String> result = null;
        try {
            result = microImageMapper.queryMicroImages();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }
}
