#include "evm_module.h"

CAN_HandleTypeDef Can_Handle[2];
static CanTxMsgTypeDef TxMessage[2];		//���ͻ�����
static CanRxMsgTypeDef RxMessage[2];		//���ջ�����

/**
  * @brief  CAN��������ж�(������) 
  * @param  hcan: CAN���ָ��
  * @retval ��
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	if(hcan->Instance == CAN1)
	{
		/* ׼���жϽ��� */
		HAL_CAN_Receive_IT(&Can_Handle[0], CAN_FIFO0);

//		printf("id:%d\r\n",Can_Handle[0].pRxMsg->StdId);
//		printf("ide:%d\r\n",Can_Handle[0].pRxMsg->IDE);
//		printf("rtr:%d\r\n",Can_Handle[0].pRxMsg->RTR);
//		printf("len:%d\r\n",Can_Handle[0].pRxMsg->DLC);
//		for(int i=0;i<8;i++)
//			printf("rxbuf[%d]:%d\r\n",i,Can_Handle[0].pRxMsg->Data[i]);
		
	}
}
/**
  * @brief  CAN����ص�����
  * @param  hcan: CAN���ָ��
  * @retval ��
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	printf("\r\nCAN����\r\n");
}

//CAN(bus, ...)
static evm_val_t evm_module_can(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc == 0 || argc>8 ) EVM_ARG_LENGTH_ERR;
	if( !evm_is_number(v)) EVM_ARG_TYPE_ERR;
	
	uint8_t bus = evm_2_integer(v);
	if(bus > 2) EVM_ARG_TYPE_ERR;
	
	evm_prop_set_value(e, p, ".bus", bus);
	
	
	return EVM_VAL_UNDEFINED;
}
//CAN.initfilterbanks(nr)
static evm_val_t evm_module_can_initfilterbanks(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.init(mode, extframe=False, prescaler=100, *, sjw=1, bs1=6, bs2=8, auto_restart=False)
static evm_val_t evm_module_can_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>8) EVM_ARG_LENGTH_ERR;
	uint32_t mode = CAN_MODE_NORMAL;
	uint32_t extframe = CAN_ID_EXT;
	uint32_t prescaler = 100;
	uint32_t sjw = 1;
	uint32_t bs1 = 6;
	uint32_t bs2 = 8;
	uint32_t auto_restart = 0;
	
	if(argc >=1) mode = evm_2_integer(v);
	if(argc >=2) 
	{
		if(evm_2_boolean(v+1) == 0) extframe = CAN_ID_STD;
		else  extframe = CAN_ID_EXT;
	}
	if(argc >=3) prescaler = evm_2_integer(v+2);
	if(argc >=4) sjw = evm_2_integer(v+3);
	if(argc >=5) bs1 = evm_2_integer(v+4);
	if(argc >=6) bs2 = evm_2_integer(v+5);
	if(argc ==7) auto_restart = evm_2_integer(v+6);

	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));
	
	if(bus == 1)
	{
		GPIO_InitTypeDef GPIO_InitStructure;   	
		/* ʹ������ʱ�� */
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/* ����CAN�������� */
		GPIO_InitStructure.Pin = GPIO_PIN_9;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Pull  = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* ����CAN�������� */
		GPIO_InitStructure.Pin = GPIO_PIN_8 ;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		/*�ж����ã���ռ���ȼ�0�������ȼ�Ϊ0*/
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 2 ,0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
		
		/************************CANͨ�Ų�������**********************************/
		/* ʹ��CANʱ�� */
		__HAL_RCC_CAN1_CLK_ENABLE();
	}else if(bus == 2){}

	Can_Handle[bus-1].Instance = CAN1;
	Can_Handle[bus-1].pTxMsg = &TxMessage[bus-1];
	Can_Handle[bus-1].pRxMsg = &RxMessage[bus-1];
	/* CAN��Ԫ��ʼ�� */
	Can_Handle[bus-1].Init.TTCM	= DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	Can_Handle[bus-1].Init.ABOM	= auto_restart;		   //MCR-ABOM  �Զ����߹��� 
	Can_Handle[bus-1].Init.AWUM	= ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	Can_Handle[bus-1].Init.NART	= DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	Can_Handle[bus-1].Init.RFLM	= DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	Can_Handle[bus-1].Init.TXFP	= DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	Can_Handle[bus-1].Init.Mode	= mode;  			   
	Can_Handle[bus-1].Init.SJW		= sjw;		   		   
	 
	Can_Handle[bus-1].Init.BS1		= bs1;		 
	Can_Handle[bus-1].Init.BS2		= bs2;		   
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 36 MHz) */
	Can_Handle[bus-1].Init.Prescaler =prescaler;		   
	HAL_CAN_Init(&Can_Handle[bus-1]);
	
	/*�ѽ��սṹ������*/
	Can_Handle[bus-1].pRxMsg->StdId = 0;
	Can_Handle[bus-1].pRxMsg->ExtId = 0;
	Can_Handle[bus-1].pRxMsg->IDE = extframe;
	Can_Handle[bus-1].pRxMsg->DLC = 0;
	Can_Handle[bus-1].pRxMsg->FMI = 0;
	for (int ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		Can_Handle[bus-1].pRxMsg->Data[ubCounter] = 0x00;
	}
	
	Can_Handle[bus-1].pTxMsg->StdId = 0;
	Can_Handle[bus-1].pTxMsg->ExtId = 0;
	Can_Handle[bus-1].pTxMsg->IDE = extframe;
	Can_Handle[bus-1].pTxMsg->IDE=CAN_ID_EXT;		 //��չģʽ
	Can_Handle[bus-1].pTxMsg->RTR=CAN_RTR_DATA;	 //���͵�������
	Can_Handle[bus-1].pTxMsg->DLC=8;							 //���ݳ���Ϊ8�ֽ�
	for (int ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		Can_Handle[bus-1].pTxMsg->Data[ubCounter] = 0x00;
	}

	
	HAL_CAN_Receive_IT(&Can_Handle[bus-1], CAN_FIFO0); 	  

	
	evm_prop_set_value(e, p, ".extframe", extframe);

	
	return EVM_VAL_UNDEFINED;
}
//CAN.deinit()
static evm_val_t evm_module_can_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));
	
	if(bus == 1)
	{
		__HAL_RCC_CAN1_CLK_DISABLE();
	}else if(bus == 2)
	{
	}	
	HAL_CAN_DeInit(&Can_Handle[bus-1]);
	return EVM_VAL_UNDEFINED;
}
//CAN.restart()
static evm_val_t evm_module_can_restart(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.state()
static evm_val_t evm_module_can_state(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));

	HAL_CAN_StateTypeDef state = HAL_CAN_GetState(&Can_Handle[bus-1]);

	return EVM_VAL_UNDEFINED;
}
//CAN.info([list])
static evm_val_t evm_module_can_info(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.setfilter(bank, mode, fifo, params, *, rtr)
static evm_val_t evm_module_can_setfilter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>5) EVM_ARG_LENGTH_ERR;
	uint32_t bank = 0;
	uint32_t mode = 0;
	uint32_t fifo = 0;
	uint8_t *rtr = NULL;

	
	if(argc >=1) bank = evm_2_integer(v);
	if(argc >=2) mode = evm_2_integer(v+1);
	if(argc >=3) fifo = evm_2_integer(v+2);
//	if(argc ==5) rtr = evm_2_integer(v+4);
	
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));
	
	if(bus == 1)
	{
		CAN_FilterConfTypeDef  CAN_FilterInitStructure;

		/*CANɸѡ����ʼ��*/
		CAN_FilterInitStructure.BankNumber = bank;
		CAN_FilterInitStructure.FilterNumber = 0;						//ɸѡ����0
		CAN_FilterInitStructure.FilterMode = (mode>>8)&0xFF;	  //����������ģʽ
		CAN_FilterInitStructure.FilterScale = mode&0xFF;	//ɸѡ��λ��Ϊ����32λ��  
		
		/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */
		if(argc ==5) {
			if(!evm_is_list(v+3)) EVM_ARG_TYPE_ERR;
			evm_val_t *params = (v+3);
			uint32_t params_len = evm_list_len(params);
			
			if(mode == ((CAN_FILTERMODE_IDLIST<<8) | CAN_FILTERSCALE_16BIT)	||
			   mode == ((CAN_FILTERMODE_IDMASK<<8) | CAN_FILTERSCALE_16BIT))
			{
				CAN_FilterInitStructure.FilterIdHigh= (*evm_list_get(e,params,0)); //Ҫɸѡ��ID��λ 
				CAN_FilterInitStructure.FilterIdLow= (*evm_list_get(e,params,1));       //Ҫɸѡ��ID��λ 
				CAN_FilterInitStructure.FilterMaskIdHigh= (*evm_list_get(e,params,2));			      //ɸѡ����16λÿλ����ƥ��
				CAN_FilterInitStructure.FilterMaskIdLow= (*evm_list_get(e,params,3));			      //ɸѡ����16λÿλ����ƥ��
			}else{
				CAN_FilterInitStructure.FilterIdHigh= ((*evm_list_get(e,params,0))>>8)&0xFF; //Ҫɸѡ��ID��λ 
				CAN_FilterInitStructure.FilterIdLow= (*evm_list_get(e,params,0))&0xFF;       //Ҫɸѡ��ID��λ 
				CAN_FilterInitStructure.FilterMaskIdHigh= ((*evm_list_get(e,params,1))>>8)&0xFF;			      //ɸѡ����16λÿλ����ƥ��
				CAN_FilterInitStructure.FilterMaskIdLow= (*evm_list_get(e,params,1))&0xFF;			      //ɸѡ����16λÿλ����ƥ��
			}
		}

		CAN_FilterInitStructure.FilterFIFOAssignment = fifo ;	//ɸѡ����������FIFO0
		CAN_FilterInitStructure.FilterActivation= ENABLE;			      //ʹ��ɸѡ��
		HAL_CAN_ConfigFilter(&Can_Handle[bus-1],&CAN_FilterInitStructure);
	}
	return EVM_VAL_UNDEFINED;
}
//CAN.clearfilter(bank)
static evm_val_t evm_module_can_clearfilter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc != 1) EVM_ARG_LENGTH_ERR;
	uint32_t bank = evm_2_integer(v);
	CAN_FilterConfTypeDef filter;

    filter.FilterIdHigh         = 0;
    filter.FilterIdLow          = 0;
    filter.FilterMaskIdHigh     = 0;
    filter.FilterMaskIdLow      = 0;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterNumber         = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_16BIT;
    filter.FilterActivation     = DISABLE;
    filter.BankNumber           = bank;

    HAL_CAN_ConfigFilter(NULL, &filter);
	return EVM_VAL_UNDEFINED;
}
//CAN.any(fifo)
static evm_val_t evm_module_can_any(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc!=1) EVM_ARG_LENGTH_ERR;
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));
	if(Can_Handle[bus-1].pRxMsg->DLC>0) return evm_mk_boolean(1);
	else return evm_mk_boolean(0);
}
//CAN.recv(fifo, list=None, *, timeout=5000)
static evm_val_t evm_module_can_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>3) EVM_ARG_LENGTH_ERR;
	uint32_t timeout = 5000;
	uint32_t fifo = evm_2_integer(v);

	if(argc == 3) timeout = evm_2_integer(v+2);
	
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));
	
	while(HAL_CAN_Receive(&Can_Handle[bus-1],0,timeout)!=HAL_TIMEOUT){};
	
	uint32_t list_len = Can_Handle[bus-1].pRxMsg->DLC;
	for(int i =0;i < list_len;i++)
	{
		evm_list_set(e,v+1,i,evm_mk_number(Can_Handle[bus-1].pRxMsg->Data[i]));
	}
	return EVM_VAL_UNDEFINED;
}
//CAN.send(data, id, *, timeout=0, rtr=False)
static evm_val_t evm_module_can_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>4) EVM_ARG_LENGTH_ERR;
	
	if(!evm_is_integer(v) && !evm_is_buffer(v)) EVM_ARG_TYPE_ERR;
	uint8_t* data = NULL;
	uint32_t data_len = 0;
	if(evm_is_number(v)) 
	{
		data = (uint8_t *) malloc(sizeof(uint8_t)*1);
		data[0] = evm_2_integer(v);
		data_len = 1;
	}else{
		data = evm_buffer_addr(v);
		data_len = evm_buffer_len(v);
		if(data_len > 8) EVM_ARG_LENGTH_ERR;
	}
	
	uint32_t id = evm_2_integer(v+1);
	uint32_t timeout = 0;
	uint32_t rtr = 0;
	if(argc >=2) timeout = evm_2_integer(v+2);
	if(argc >=3) {
		rtr = (evm_2_boolean(v+3)==0)?CAN_RTR_DATA:CAN_RTR_REMOTE;
	}
	
	uint8_t bus = evm_2_integer(evm_prop_get(e, p, ".bus", 0));

	Can_Handle[bus-1].pTxMsg->StdId = id;						 
	Can_Handle[bus-1].pTxMsg->ExtId = id;			 	//ʹ�õ���չID
	Can_Handle[bus-1].pTxMsg->IDE = CAN_ID_EXT;     	//��չģʽ
	Can_Handle[bus-1].pTxMsg->RTR = rtr;	 		 	//����&Զ��֡
	Can_Handle[bus-1].pTxMsg->DLC = data_len;			//���ݳ���Ϊ8�ֽ�

	/*����Ҫ���͵�����0-7*/
	for (int i = 0; i < data_len; i++)
	{
		Can_Handle[bus-1].pTxMsg->Data[i] = data[i];
	}
	HAL_CAN_Transmit(&Can_Handle[bus-1],timeout);
	
	free(data);
	return EVM_VAL_UNDEFINED;
}
//CAN.rxcallback(fifo, fun)
static evm_val_t evm_module_can_rxcallback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_can(evm_t * e){
	evm_builtin_t class_can[] = {
		{"initfilterbanks", evm_mk_native( (intptr_t)evm_module_can_initfilterbanks )},
		{"init", evm_mk_native( (intptr_t)evm_module_can_init )},
		{"deinit", evm_mk_native( (intptr_t)evm_module_can_deinit )},
		{"restart", evm_mk_native( (intptr_t)evm_module_can_restart )},
		{"state", evm_mk_native( (intptr_t)evm_module_can_state )},
		{"info", evm_mk_native( (intptr_t)evm_module_can_info )},
		{"setfilter", evm_mk_native( (intptr_t)evm_module_can_setfilter )},
		{"clearfilter", evm_mk_native( (intptr_t)evm_module_can_clearfilter )},
		{"any", evm_mk_native( (intptr_t)evm_module_can_any )},
		{"recv", evm_mk_native( (intptr_t)evm_module_can_recv )},
		{"send", evm_mk_native( (intptr_t)evm_module_can_send )},
		{"rxcallback", evm_mk_native( (intptr_t)evm_module_can_rxcallback )},
		{"NORMAL",evm_mk_number((uint32_t)CAN_MODE_NORMAL)},
		{"LOOPBACK",evm_mk_number((uint32_t)CAN_MODE_LOOPBACK)},
		{"SILENT",evm_mk_number((uint32_t)CAN_MODE_SILENT)},
		{"SILENT_LOOPBACK",evm_mk_number((uint32_t)CAN_MODE_SILENT_LOOPBACK)},
		{"STOPPED",evm_mk_number(0)},
		{"ERROR_ACTIVE",evm_mk_number(1)},
		{"ERROR_WARNING",evm_mk_number(2)},
		{"ERROR_PASSIVE",evm_mk_number(3)},
		{"BUS_OFF",evm_mk_number(4)},
		{"LIST16",evm_mk_number((uint16_t)(CAN_FILTERMODE_IDLIST<<8) | CAN_FILTERSCALE_16BIT)},
		{"MASK16",evm_mk_number((uint16_t)(CAN_FILTERMODE_IDMASK<<8) | CAN_FILTERSCALE_16BIT)},
		{"LIST32",evm_mk_number((uint16_t)(CAN_FILTERMODE_IDLIST<<8) | CAN_FILTERSCALE_32BIT)},
		{"MASK32",evm_mk_number((uint16_t)(CAN_FILTERMODE_IDMASK<<8) | CAN_FILTERSCALE_32BIT)},
		
		{".bus",evm_mk_number(0)},
		{".extframe",evm_mk_number(CAN_ID_EXT)},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_can, class_can, NULL);
}

