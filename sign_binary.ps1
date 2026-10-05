$ExePath = "C:\Users\HP\source\repos\Hitlar\x64\Debug\Hitlar.exe"
$cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1
if ($cert) {
    Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert
    Write-Host "Signed with Set-AuthenticodeSignature"
} else {
    Write-Error "No CodeSigning cert found"
}
