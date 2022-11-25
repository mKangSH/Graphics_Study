## Error 1
- 문제: 코드를 수정하는 과정에서 루트 서명을 만드는 데 사용될 루트 매개변수 배열의 크기를 잘못된 상태로 방치한 문제
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/AllocateWrongArraySize.PNG" title=""></img>   
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/E_INVALIDARG.PNG" title=""></img>  
- 해결: 배열의 크기를 루트 매개변수가 정의된 개수(2개)로 수정하여 진행

## Error 2 (연관되어 있는 변수를 모두 확인하자)
- 문제: 파이프라인 상태 개체와 루트 서명의 초기화 값이 일치하지 않는 문제     
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/NotPopulateSamplerArgument.PNG" title=""></img>      
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/CallingStack.PNG" title=""></img>      
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/E_INVALIDARG.PNG" title=""></img>   
- 해결: 문제가 발생한 에러 스택 지점 확인   
    - 마지막 스택인 ThrowIfFailed()는 문제가 생겼을 때 std::exception()을 던지는 함수   
    - AssetInit() 함수 내부인 line: 202에 중단점을 걸고 Visual Studio local debugger를 실행   
    - 문제 위치를 확인한 결과 CreateGraphicsPipelineState() 함수에서 오류가 발생
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/CallingStack.PNG" title=""></img>    
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/CreateGraphicsPipelineState.PNG" title=""></img>    
    - PSO(Pipeline State Object)에 속한 샘플러의 개수를 지정하는 변수인 (SampleDesc.Count) 의 값과 루트 서명의 초기화 값이 동일하지 않아서 발생했단 사실 인지   
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/CreateGraphicsPipelineState.PNG" title=""></img>     
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/3.%20Moving%20Shape%20Populated%20by%20Texture/NotPopulateSamplerArgument.PNG" title=""></img>