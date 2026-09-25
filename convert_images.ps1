Add-Type -AssemblyName System.Drawing

$files = Get-ChildItem -Path "object_images\*.bmp"
foreach ($f in $files) {
    try {
        $img = [System.Drawing.Image]::FromFile($f.FullName)
        $pngPath = [System.IO.Path]::ChangeExtension($f.FullName, ".png")
        $img.Save($pngPath, [System.Drawing.Imaging.ImageFormat]::Png)
        $img.Dispose()
        Remove-Item -Path $f.FullName -Force
        Write-Host "Converted: $($f.Name) -> $([System.IO.Path]::GetFileName($pngPath))"
    } catch {
        Write-Warning "Failed on $($f.FullName): $_"
    }
}
