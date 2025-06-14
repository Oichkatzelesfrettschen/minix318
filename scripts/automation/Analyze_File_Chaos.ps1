# MINIX4 File Analysis and Restructuring Plan
# Simple script to analyze the current chaos and show restructuring plan

Write-Host "=" * 80 -ForegroundColor Magenta
Write-Host " MINIX4 FILE CHAOS ANALYSIS" -ForegroundColor Magenta  
Write-Host "=" * 80 -ForegroundColor Magenta

Write-Host "`n🔍 ANALYZING CURRENT FILE CHAOS..." -ForegroundColor Cyan

# Count files in root directory
$RootFiles = Get-ChildItem -File | Where-Object { $_.Extension -eq ".c" -or $_.Extension -eq ".h" -or $_.Extension -eq ".s" }
Write-Host "📊 Files in root directory: $($RootFiles.Count)" -ForegroundColor Yellow

# Show specific problematic files
Write-Host "`n🚨 CRITICAL FILES MISPLACED IN ROOT:" -ForegroundColor Red
$CriticalFiles = @("clock.c", "main.c", "proc.c", "malloc.c", "string.c", "printf.c")
foreach ($File in $CriticalFiles) {
    if (Test-Path $File) {
        Write-Host "  ❌ $File (SHOULD BE IN KERNEL/LIBOS)" -ForegroundColor Red
    }
}

# Show YACC files pollution
Write-Host "`n🗑️ LEGACY YACC FILES POLLUTING ROOT:" -ForegroundColor Yellow
$YaccFiles = Get-ChildItem -Filter "yy*.c" 
$YaccFiles += Get-ChildItem -Filter "y.tab.*"
Write-Host "  📋 YACC files found: $($YaccFiles.Count)" -ForegroundColor Yellow
foreach ($File in $YaccFiles) {
    Write-Host "    • $($File.Name) → archive/unix_v6v7/yacc/" -ForegroundColor Yellow
}

# Show Pascal files
Write-Host "`n📚 PASCAL INTERPRETER FILES:" -ForegroundColor Cyan
$PascalFiles = @("lookup.c", "lval.c", "rec.c", "rval.c", "tree.c", "type.c", "var.c")
foreach ($File in $PascalFiles) {
    if (Test-Path $File) {
        Write-Host "    • $File → archive/unix_v6v7/pascal/" -ForegroundColor Cyan
    }
}

# Show demo files
Write-Host "`n🎮 DEMO AND TEST FILES:" -ForegroundColor Green
$DemoFiles = Get-ChildItem -Filter "*_demo.c"
$TestFiles = Get-ChildItem -Filter "*_test.c"
Write-Host "  🎯 Demo files: $($DemoFiles.Count)" -ForegroundColor Green
Write-Host "  🧪 Test files: $($TestFiles.Count)" -ForegroundColor Green

# Show assembly files
Write-Host "`n⚙️ ASSEMBLY FILES IN ROOT:" -ForegroundColor Magenta
$AsmFiles = Get-ChildItem -Filter "*.s"
Write-Host "  📋 Assembly files: $($AsmFiles.Count)" -ForegroundColor Magenta
foreach ($File in $AsmFiles) {
    Write-Host "    • $($File.Name) → minix4/exokernel/arch/i386/" -ForegroundColor Magenta
}

Write-Host "`n" + "=" * 80 -ForegroundColor Green
Write-Host " PROPOSED RESTRUCTURING SOLUTION" -ForegroundColor Green
Write-Host "=" * 80 -ForegroundColor Green

Write-Host "`n🏗️ NEW CLEAN ARCHITECTURE:" -ForegroundColor Cyan
Write-Host "minix4/" -ForegroundColor White
Write-Host "├── exokernel/          # Hardware abstraction layer" -ForegroundColor Gray
Write-Host "├── microkernel/        # System services layer" -ForegroundColor Gray  
Write-Host "├── libos/              # POSIX compatibility layer" -ForegroundColor Gray
Write-Host "└── include/            # Global headers" -ForegroundColor Gray
Write-Host "userspace/" -ForegroundColor White
Write-Host "├── commands/           # System utilities" -ForegroundColor Gray
Write-Host "└── applications/       # User applications" -ForegroundColor Gray
Write-Host "archive/" -ForegroundColor White
Write-Host "├── unix_v6v7/          # Historical UNIX code" -ForegroundColor Gray
Write-Host "└── minix_legacy/       # Original MINIX for reference" -ForegroundColor Gray

Write-Host "`n🎯 KEY MOVES REQUIRED:" -ForegroundColor Yellow
Write-Host "• clock.c → minix4/microkernel/core/timing/" -ForegroundColor White
Write-Host "• proc.c → minix4/microkernel/core/process/" -ForegroundColor White
Write-Host "• malloc.c → minix4/libos/posix/libc/stdlib/" -ForegroundColor White
Write-Host "• string.c → minix4/libos/posix/libc/string/" -ForegroundColor White
Write-Host "• yy*.c → archive/unix_v6v7/yacc/" -ForegroundColor White
Write-Host "• *_demo.c → userspace/applications/demos/" -ForegroundColor White
Write-Host "• *.s → minix4/exokernel/arch/i386/" -ForegroundColor White

Write-Host "`n💡 TO EXECUTE RESTRUCTURING:" -ForegroundColor Cyan
Write-Host "1. Review this analysis" -ForegroundColor White
Write-Host "2. Create the new directory structure manually" -ForegroundColor White  
Write-Host "3. Move files according to the plan above" -ForegroundColor White
Write-Host "4. Update build system for new structure" -ForegroundColor White

Write-Host "`n⚠️  IMPACT: This will transform chaos into clean architecture!" -ForegroundColor Green
Write-Host "✅ Ready to proceed with manual restructuring?" -ForegroundColor Green

Write-Host "`n" + "=" * 80 -ForegroundColor Magenta
