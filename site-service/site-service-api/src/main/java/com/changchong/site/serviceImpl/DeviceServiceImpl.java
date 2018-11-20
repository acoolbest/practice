package com.changchong.site.serviceImpl;

import java.math.BigDecimal;
import java.util.*;

import com.changchong.site.mapper.PortLogMapper;
import com.changchong.site.model.PortLog;
import com.changchong.site.pay.dto.*;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.changchong.sdk.constants.DeviceConstants;
import com.changchong.site.mapper.DeviceMapper;
import com.changchong.site.mapper.PortMapper;
import com.changchong.site.model.Device;
import com.changchong.site.model.Port;
import com.changchong.site.service.DeviceService;
import com.changchong.util.ConfigUtil;

@Service(value = "deviceService")
public class DeviceServiceImpl implements DeviceService{

	@Autowired
	private DeviceMapper deviceMapper;
	@Autowired
	private PortMapper portMapper;
	@Autowired
	private PortLogMapper portLogMapper;
	
	@Override
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	//当设备部存在或者端口信息未上传时候调用改方法
	public DeviceInfoResultDto insertDeviceInfo(DeviceInfoDto deviceInfoDto,Integer deviceId) {
		DeviceInfoResultDto deviceResultDto = new DeviceInfoResultDto();
		List<PortCodeDto> devicePortCode = new ArrayList<PortCodeDto>();
		if(deviceId==0){
			Device deviceNew = new Device();
			deviceNew.setCode(deviceInfoDto.getDeviceName());
			deviceNew.setStatus(DeviceConstants.DEVICE_STATUS_OK);
			deviceNew.setProvince(Integer.parseInt(deviceInfoDto.getProvince()));
			deviceMapper.insertDevice(deviceNew);
			deviceId =deviceNew.getId(); 
		}else{
			Device deviceNew = new Device();
			deviceNew.setId(deviceId);
			deviceNew.setProvince(Integer.parseInt(deviceInfoDto.getProvince()));
			deviceMapper.updateDevice(deviceNew);
		}
		updateDeviceInfo(deviceInfoDto,deviceId,devicePortCode);
		/*if(deviceInfoDto.getPortCode()!=null&&deviceInfoDto.getPortCode().size()>0){
			devicePortCode = deviceInfoDto.getPortCode();
			int count =Integer.parseInt(deviceInfoDto.getPortCount())-deviceInfoDto.getPortCode().size(); 
			for(int i=0;i<count;i++){
				Port portInsert = new Port();
				portInsert.setDeviceId(deviceId);
				portInsert.setStatus(DeviceConstants.PORT_STATUS_OK);
				portMapper.insertPort(portInsert);
				PortCodeDto portCode = new PortCodeDto();
				portCode.setCode(portInsert.getId()+"");
				devicePortCode.add(portCode);
			}
		}else{
			for(int i=0;i<Integer.parseInt(deviceInfoDto.getPortCount());i++){
				Port portInsert = new Port();
				portInsert.setDeviceId(deviceId);
				portInsert.setStatus(DeviceConstants.PORT_STATUS_OK);
				portMapper.insertPort(portInsert);
				PortCodeDto portCode = new PortCodeDto();
				portCode.setCode(portInsert.getId()+"");
				devicePortCode.add(portCode);
			}
		}*/
		deviceResultDto.setProvince(deviceInfoDto.getProvince());
		deviceResultDto.setDeviceName(deviceInfoDto.getDeviceName());
		deviceResultDto.setPortCount(deviceInfoDto.getPortCount());
		deviceResultDto.setPortCode(devicePortCode);
		deviceResultDto.setCodeUrl(ConfigUtil.getPayValue("port_return_url"));
		return deviceResultDto;		
	}


	public DeviceInfoDto getDeviceByName(String deviceName) {
		DeviceInfoDto deviceInfoDto = new DeviceInfoDto();
		Device device = deviceMapper.getDeviceByName(deviceName);
		if(device!=null){
			deviceInfoDto.setDeviceName(device.getCode());
			List<Port> ports = portMapper.getPortByDeviceId(device.getId());
			deviceInfoDto.setPortCount(ports.size()+"");
			List<PortCodeDto> listPort = new ArrayList<PortCodeDto>();
			for(Port port:ports){ 
				PortCodeDto portDto = new PortCodeDto();
				portDto.setCode(port.getId()+"");
				listPort.add(portDto);
			}
			deviceInfoDto.setPortCode(listPort);
			return deviceInfoDto;
		}else{
			return null;
		}
	}


	//设备信息更新即更新设备端口信息，因为端口编号是唯一的，需要先检查设备此次更新的设备信息中的端口是否有其他设备已经在使用，如果有则先删除，然后再插入
	public void updateDeviceInfo(DeviceInfoDto deviceInfoDto,Integer deviceId,List<PortCodeDto> devicePortCode) {
		List<PortCodeDto> portCodes = deviceInfoDto.getPortCode();//获取上传的device下的端口		
		int portCount = Integer.parseInt(deviceInfoDto.getPortCount());//获取要生成的端口数量
		int addNumer=0;//用来计数的
		int flag =0;//用来标记的
		if(portCodes!=null&&portCodes.size()>0){
			for(PortCodeDto porNew:portCodes){//上传的端口，首先需要检查是否有其他机器已经使用该端口，如果有使用则先删除，然后再插入
				Port portQuery = portMapper.getPortById(Integer.parseInt(porNew.getCode()));//code即为portid
				if(portQuery!=null&&portQuery.getId()!=null&&portQuery.getDeviceId()!=null&&portQuery.getDeviceId().intValue()==deviceId.intValue()){
					portMapper.deletePort(portQuery);
					Port portInsert = new Port();
					portInsert.setId(Integer.parseInt(porNew.getCode()));
					portInsert.setDeviceId(deviceId);
					portInsert.setStatus(DeviceConstants.PORT_STATUS_OK);//初始上传数据设备端口信息均为正常
					portMapper.insertPort(portInsert);
					devicePortCode.add(porNew);
					addNumer++;
				}				
			}
		}
		List<Port> ports = portMapper.getPortByDeviceId(deviceId);//获取该设备下所有端口
		for(Port portOld:ports){
			for(PortCodeDto uploadCode:devicePortCode){
				if(portOld.getId().intValue()==Integer.parseInt(uploadCode.getCode())){
					flag =1;
					break;
				}								
			}
			if(flag==1){//此次循环获取到的port在deviceportCode里面已经存在
				flag =0;
				continue;
			}
			if(addNumer<portCount){
				PortCodeDto porAdd = new PortCodeDto();
				porAdd.setCode(portOld.getId()+"");
				devicePortCode.add(porAdd);
				addNumer++;
			}else{
				portMapper.deletePortFromDevice(portOld);//解除端口和设备关系
			}
		}
		for(int i=0;i<portCount-addNumer;i++){//依旧不满足的情况
			Port portInsert = new Port();
			portInsert.setDeviceId(deviceId);
			portInsert.setStatus(DeviceConstants.PORT_STATUS_OK);
			portMapper.insertPort(portInsert);
			PortCodeDto portCode = new PortCodeDto();
			portCode.setCode(portInsert.getId()+"");
			devicePortCode.add(portCode);
		}
	}


	@Override
	public DeviceDto findDeviceByName(String code) {
		DeviceDto deviceDto = new DeviceDto();
		Device device = deviceMapper.getDeviceByName(code);
		if(device==null)
			return null;
		BeanUtils.copyProperties(device, deviceDto);
		return deviceDto;
	}


	@Override
	public String finadDeviceByPort(Integer portNumber) {
		return deviceMapper.finadDeviceByPort(portNumber);
	}


	@Override
	public List<DeviceDto> getAllDevice() {
		List<DeviceDto> deviceDtos = new ArrayList<DeviceDto>();
		List<Device> devices = deviceMapper.getAllDevice();
		for(Device device:devices){
			DeviceDto deviceDto = new DeviceDto();
			BeanUtils.copyProperties(device, deviceDto);
			deviceDtos.add(deviceDto);
		}
		return deviceDtos;
	}


	
	@Override
	public List<Integer> selectId(Map<String, Object> param) {
		List<Integer> list = deviceMapper.selectId(param);
		
		param.clear();
		param.put("deviceIds", list);
		List<Integer> list1 = portMapper.selectId(param);
		return list1;
	}

	@Override
	public DeviceDto getDeviceById(Integer id) {
		Device device = deviceMapper.getDeviceById(id);
		if(device==null){
			return null;
		}
		DeviceDto deviceDto = new DeviceDto();
		BeanUtils.copyProperties(device,deviceDto);
		deviceDto.setCode(device.getDeviceCode());
		return deviceDto;
	}

    @Override
    public void savePortLog(PortElectricDto portElectricDto) {
		if (portElectricDto != null) {
			PortLog portLog = new PortLog();
			portLog.setPortId(portElectricDto.getPortCode());
			portLog.setCurrentElectric(portElectricDto.getCurrentElectric());
			portLog.setCreateTime(new Date());
			portLogMapper.insertSelective(portLog);
		}

    }


}
