const logArea = document.getElementById('logArea');
const errorElement = document.getElementById('error');
const attendanceTableBody = document.getElementById('attendanceTableBody');
let attendanceData = [];

function updateAttendanceTable() {
  const dailyMaxAttendance = attendanceData.reduce((acc, log) => {
    const date = new Date(log.timestamp).toLocaleDateString();
    if (!acc[date] || log.count > acc[date]) {
      acc[date] = log.count;
    }
    return acc;
  }, {});

  const sortedDates = Object.keys(dailyMaxAttendance).sort((a, b) => new Date(b) - new Date(a));
  const last10Dates = sortedDates.slice(0, 10);

  attendanceTableBody.innerHTML = '';
  last10Dates.forEach(date => {
    const row = document.createElement('tr');
    row.innerHTML = `
      <td>${date}</td>
      <td>${dailyMaxAttendance[date]}</td>
    `;
    attendanceTableBody.appendChild(row);
  });
}

function updateLogArea() {
  logArea.innerHTML = '';
  attendanceData.slice(0, 10).forEach(log => {
    const logEntry = document.createElement('div');
    logEntry.className = 'log-entry';
    logEntry.innerHTML = `
      <span><strong>Count:</strong> ${log.count}</span>
      <span class="timestamp">${new Date(log.timestamp).toLocaleString()}</span>
    `;
    logArea.appendChild(logEntry);
  });
}

async function fetchAttendanceData() {
  try {
    const response = await fetch('http://localhost:3000/api/attendance');
    if (!response.ok) throw new Error('Failed to fetch attendance data');
    
    const data = await response.json();
    errorElement.style.display = 'none';
    
    attendanceData = [...data, ...attendanceData].slice(0, 100); // Keep last 100 entries
    
    updateLogArea();
    updateAttendanceTable();
  } catch (error) {
    console.error('Error:', error);
    errorElement.textContent = 'Failed to load attendance data. Will retry in 2 minutes.';
    errorElement.style.display = 'block';
  }
}

// Initial fetch
fetchAttendanceData();

// Fetch every 2 minutes
setInterval(fetchAttendanceData, 120000);