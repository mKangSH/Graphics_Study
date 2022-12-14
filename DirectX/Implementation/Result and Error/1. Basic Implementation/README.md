## Error 1
- 문제: 셰이더 파일의 진입점과 컴파일러의 진입점 설정이 다른 경우(X3501)     
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/1.%20Basic%20Implementation/Error1.PNG" title=""></img>
- 해결: 컴파일러 옵션에서 디폴트 'main'으로 되어 있는 진입점을 셰이더 파일의 진입점 이름과 통일

## Error 2
- 문제: 셰이더 파일의 경로와 셰이더 컴파일 경로가 일치하지 않는 경우    
<img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/1.%20Basic%20Implementation/Error3.PNG" title=""></img>   
- 해결: 문제가 발생한 에러 스택 지점 확인   
    - 마지막 스택인 ThrowIfFailed()는 문제가 생겼을 때 std::exception()을 던지는 함수   
    - AssetInit() 함수 내부인 line: 133에 중단점을 걸고 Visual Studio local debugger를 실행   
    - 경로를 가져오는 GetAssetFullPath() 함수 내부 리턴 값: m_assetsPath + assetName 두 변수를 조사   
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/1.%20Basic%20Implementation/Error%20Stack.PNG" title=""></img>    
      <img src="https://github.com/mKangSH/Graphics_Study/blob/main/DirectX/Implementation/Result%20and%20Error/1.%20Basic%20Implementation/Watch%20Window.PNG" title=""></img>
    - m_assetsPath에 해당하는 폴더에 "shaders.hlsl" 파일을 추가하여 해결
    - GetAssetFullPath() 함수 대신 std::wstring 을 이용하여 직접 경로 변수를 정의하여 인자로 대입하여 해결   
        ```
        std::wstring shaderFilePath = L"./shaders.hlsl" // 직접 경로 
        D3DCompileFromFile(shaderFilePath.c_str(), ...);
        ```
